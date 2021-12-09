/*Kernel module to create a DiskonFile block device */
#include <linux/version.h> 	
#include <linux/blk-mq.h>	
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>	
#include <linux/slab.h>		
#include <linux/fs.h>		
#include <linux/errno.h>	
#include <linux/types.h>	
#include <linux/kdev_t.h>
#include <linux/vmalloc.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/buffer_head.h>
#include <linux/bio.h>

static int blk_major = 0;
static int sector_size = 512;
static int num_sector = 1024;	/* Total no of sectors in the block device */

/*Number of minors for the block device*/
#define NUM_MINOR 2

/*Parameters to create the partitions for the device*/
#define KERNEL_SECTOR_SIZE 512
#define MBR_SIZE KERNEL_SECTOR_SIZE
#define MBR_DISK_SIGNATURE_OFFSET 440
#define MBR_DISK_SIGNATURE_SIZE 4
#define PARTITION_TABLE_OFFSET 446
#define PARTITION_ENTRY_SIZE 16 
#define PARTITION_TABLE_SIZE 64 
#define MBR_SIGNATURE_OFFSET 510
#define MBR_SIGNATURE_SIZE 2
#define MBR_SIGNATURE 0xAA55

typedef struct
{
	unsigned char boot_type; // 0x00 - Inactive; 0x80 - Active (Bootable)
	unsigned char start_head;
	unsigned char start_sec:6;
	unsigned char start_cyl_hi:2;
	unsigned char start_cyl;
	unsigned char part_type;
	unsigned char end_head;
	unsigned char end_sec:6;
	unsigned char end_cyl_hi:2;
	unsigned char end_cyl;
	unsigned int abs_start_sec;
	unsigned int sec_in_part;
} PartEntry;

typedef PartEntry PartTable[4];

//Partition parameter declaration
static PartTable def_part_table =
{
	{
		boot_type: 0x00,
		start_head: 0x00,
		start_sec: 0x2,
		start_cyl: 0x00,
		part_type: 0x83,
		end_head: 0x00,
		end_sec: 0x20,
		end_cyl: 0x09,
		abs_start_sec: 0x00000001,
		sec_in_part: 0x0000013F
	},
	{
		boot_type: 0x00,
		start_head: 0x00,
		start_sec: 0x1,
		start_cyl: 0x14,
		part_type: 0x83,
		end_head: 0x00,
		end_sec: 0x20,
		end_cyl: 0x1F,
		abs_start_sec: 0x00000280,
		sec_in_part: 0x00000180
	},
	{	
	},
	{
	}
};

struct blkdev {
        int size;                       /* Device size in sectors */
        u8 *data;                       /* The data array */
        spinlock_t lock;                /* For mutual exclusion */
		struct blk_mq_tag_set tag_set;	/* tag_set added */
        struct request_queue *queue;    /* The device request queue */
        struct gendisk *gd;             /* The gendisk structure */
}device;


static void blkdev_transfer(struct blkdev *dev, unsigned long sector,
		unsigned long num_sect, char *buffer, int rd_wr)
{
	unsigned long offset = sector*KERNEL_SECTOR_SIZE;
	unsigned long num_bytes = num_sect*KERNEL_SECTOR_SIZE;

	if ((offset + num_bytes) > dev->size) {
		printk (KERN_NOTICE "Beyond-end write (%ld %ld)\n", offset, num_bytes);
		return;
	}
	if (rd_wr)		//If 1 then write othrwise do read
		memcpy(dev->data + offset, buffer, num_bytes);
	else
		memcpy(buffer, dev->data + offset, num_bytes);
}

//Function to add the details of the partition to the MBR 
static void copy_mbr(u8 *disk)
{
	memset(disk, 0x0, MBR_SIZE);
	*(unsigned long *)(disk + MBR_DISK_SIGNATURE_OFFSET) = 0x36E5756D;
	memcpy(disk + PARTITION_TABLE_OFFSET, &def_part_table, PARTITION_TABLE_SIZE);
	*(unsigned short *)(disk + MBR_SIGNATURE_OFFSET) = MBR_SIGNATURE;
}

//Conditional loop to check for compatibility with different kernel versions of Linux

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 9, 0))
static inline struct request_queue *
blk_generic_alloc_queue(make_request_fn make_request, int node_id)
#else
static inline struct request_queue *
blk_generic_alloc_queue(int node_id)
#endif
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 7, 0))
	struct request_queue *q = blk_alloc_queue(GFP_KERNEL);
	if (q != NULL)
		blk_queue_make_request(q, make_request);

	return (q);
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(5, 9, 0))
	return (blk_alloc_queue(make_request, node_id));
#else
	return (blk_alloc_queue(node_id));
#endif
}

//Function to create the bio vector to handle multiple transfer requests
static blk_status_t blkdev_request(struct blk_mq_hw_ctx *hctx, const struct blk_mq_queue_data* bd)   /* For blk-mq */
{
	struct request *req = bd->rq;
	struct blkdev *dev = req->rq_disk->private_data;
    struct bio_vec bvec;
    struct req_iterator iter;
    sector_t pos_sector = blk_rq_pos(req);

	void *buffer;
	blk_status_t  ret;

	blk_mq_start_request (req);

	if (blk_rq_is_passthrough(req)) {
		printk (KERN_NOTICE "Skip non-fs request\n");
                ret = BLK_STS_IOERR;  //-EIO
			goto done;
	}
	rq_for_each_segment(bvec, req, iter)
	{
		size_t num_sector = blk_rq_cur_sectors(req);
		printk (KERN_NOTICE "dir %d sec %lld, nr %ld\n",
                        rq_data_dir(req),
                        pos_sector, num_sector);
		buffer = page_address(bvec.bv_page) + bvec.bv_offset;
		blkdev_transfer(dev, pos_sector, num_sector,
				buffer, rq_data_dir(req) == WRITE);
		pos_sector += num_sector;
	}
	ret = BLK_STS_OK;
done:
	blk_mq_end_request (req, ret);
	return ret;
}


/*Function to handle a single BIO. */
static int blkdev_xfer_bio(struct blkdev *dev, struct bio *bio)
{
	struct bio_vec bvec;
	struct bvec_iter iter;
	sector_t sector = bio->bi_iter.bi_sector;

	/* Handlind each transfer request independently*/
	bio_for_each_segment(bvec, bio, iter) {
		
		char *buffer = kmap_atomic(bvec.bv_page) + bvec.bv_offset;
		blkdev_transfer(dev, sector, (bio_cur_bytes(bio) / KERNEL_SECTOR_SIZE),
				buffer, bio_data_dir(bio) == WRITE);
		sector += (bio_cur_bytes(bio) / KERNEL_SECTOR_SIZE);
		kunmap_atomic(buffer);
	}
	return 0;
}

/*Function to handle every request to the block device*/
static int blkdev_xfer_request(struct blkdev *dev, struct request *req)
{
	struct bio *bio;
	int num_sect = 0;
    
	__rq_for_each_bio(bio, req) {
		blkdev_xfer_bio(dev, bio);
		num_sect += bio->bi_iter.bi_size/KERNEL_SECTOR_SIZE;
	}
	return num_sect;
}


static int blkdev_open(struct block_device *bdev, fmode_t mode)	 
{
	int ret=0;
	printk(KERN_INFO "mydiskdrive : open \n");
	goto out;

	out :
	return ret;

}

static void blkdev_release(struct gendisk *disk, fmode_t mode)
{
	
	printk(KERN_INFO "mydiskdrive : closed \n");

}

static struct block_device_operations blk_ops =
{
	.owner = THIS_MODULE,
	.open = blkdev_open,
	.release = blkdev_release,
};

static struct blk_mq_ops mq_ops_simple = {
    .queue_rq = blkdev_request,
};

static int __init blkdev_init(void)
{
	blk_major = register_blkdev(blk_major, "dof");
	if (blk_major <= 0) {
		printk(KERN_INFO "sbull: unable to get major number\n");
		return -EBUSY;
	}
        struct blkdev* dev = &device;

	//setup partition table
	device.size = num_sector*sector_size;
	device.data = vmalloc(device.size);
	copy_mbr(device.data);
	spin_lock_init(&device.lock);	
		
	device.queue = blk_mq_init_sq_queue(&device.tag_set, &mq_ops_simple, 128, BLK_MQ_F_SHOULD_MERGE);
	blk_queue_logical_block_size(device.queue, sector_size);
	(device.queue)->queuedata = dev;
	device.gd = alloc_disk(NUM_MINOR);
	device.gd->major = blk_major;
	device.gd->first_minor = 0;
	device.gd->minors = NUM_MINOR;
	device.gd->fops = &blk_ops;
	device.gd->queue = dev->queue;
	device.gd->private_data = dev;
	sprintf(device.gd->disk_name,"dof");
	set_capacity(device.gd, num_sector*(sector_size/KERNEL_SECTOR_SIZE));
	add_disk(device.gd);	    
	return 0;
}

static void blkdev_exit(void)
{
	del_gendisk(device.gd);
	unregister_blkdev(blk_major, "diskonfile");
	put_disk(device.gd);	
	blk_cleanup_queue(device.queue);
	vfree(device.data);
	spin_unlock(&device.lock);	
	printk(KERN_ALERT "diskonfile is unregistered");
}
	
module_init(blkdev_init);
module_exit(blkdev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nived Suresh");
MODULE_DESCRIPTION("DiskonFile Block Device");
