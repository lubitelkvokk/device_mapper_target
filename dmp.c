
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/bio.h>
#include <linux/device-mapper.h>
#include <linux/fs.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>

struct my_dm_target
{
  struct dm_dev *dev;
  sector_t start;
};
static size_t req_count_write = 0;
static size_t bytes_written = 0;
static size_t req_count_read = 0;
static size_t bytes_read = 0;

static int dmp_map(struct dm_target *ti, struct bio *bio)
{
  struct my_dm_target *mdt = (struct my_dm_target *)ti->private;
  printk(KERN_CRIT "\n<<in function dmp_map \n");

  if (bio_data_dir(bio) == WRITE)
  {
    bytes_written += bio->bi_iter.bi_size;
    req_count_write++;
    printk(KERN_CRIT "\n dmp_map : bio is a write request.... \n");
  }
  else
  {
    bytes_read += bio->bi_iter.bi_size;
    req_count_read++;
    printk(KERN_CRIT "\n dmp_map : bio is a read request.... \n");
  }

  bio_set_dev(bio, mdt->dev->bdev);
  bio->bi_iter.bi_sector += mdt->start;

  submit_bio(bio);

  printk(KERN_CRIT "\n>>out function dmp_map \n");
  return DM_MAPIO_SUBMITTED;
}

static int
dmp_ctr(struct dm_target *ti, unsigned int argc, char **argv)
{
  struct my_dm_target *mdt;

  printk(KERN_CRIT "\n >>in function dmp_ctr \n");
  printk(KERN_CRIT "\n >> ARGUMENTS %d", argc);

  if (argc != 1)
  {
    printk(KERN_CRIT "\n Invalid no.of arguments.\n");
    ti->error = "Invalid argument count";
    return -EINVAL;
  }

  mdt = kzalloc(sizeof(struct my_dm_target), GFP_KERNEL);
  mdt->start = 0; // TODO changing start

  if (mdt == NULL)
  {
    printk(KERN_CRIT "\n Mdt is null\n");
    ti->error = "dm-dmp: Cannot allocate linear context";
    return -ENOMEM;
  }

  if (dm_get_device(ti, argv[0], dm_table_get_mode(ti->table), &mdt->dev))
  {
    ti->error = "dm-dmp: Device lookup failed";
    goto bad;
  }

  ti->private = mdt;

  printk(KERN_CRIT "\n>>out function dmp_ctr \n");
  return 0;

bad:
  kfree(mdt);
  printk(KERN_CRIT "\n>>out function dmp_ctr with errorrrrrrrrrr \n");
  return -EINVAL;
}

static void dmp_dtr(struct dm_target *ti)
{
  struct my_dm_target *mdt = (struct my_dm_target *)ti->private;
  printk(KERN_CRIT "\n<<in function dmp_dtr \n");
  dm_put_device(ti, mdt->dev);
  kfree(mdt);
  printk(KERN_CRIT "\n>>out function dmp_dtr \n");
}

static struct target_type dmp = {

    .name = "dmp",
    .version = {1, 0, 0},
    .module = THIS_MODULE,
    .ctr = dmp_ctr,
    .dtr = dmp_dtr,
    .map = dmp_map,
};

/* SYSFS*/

static struct kobject *dmp_sysfs;

static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
  size_t avg_size_read = 0;
  size_t avg_size_write = 0;
  size_t total_req_count = 0;
  size_t avg_size = 0;
  if (req_count_read != 0)
    avg_size_read = bytes_read / req_count_read;
  if (req_count_write != 0)
    avg_size_write = bytes_written / req_count_write;
  total_req_count = (req_count_read + req_count_write);
  if (total_req_count != 0)
    avg_size = (bytes_read + bytes_written) / total_req_count;
  return sprintf(buf, "read:\n\treqs: %lu\n\tavg size: %lu\n"
                      "write:\n\treqs: %lu\n\tavg size:%lu\n"
                      "total:\n\treqs: %lu\n\tavg size:%lu\n",
                 req_count_read, avg_size_read,
                 req_count_write, avg_size_write,
                 total_req_count, avg_size);
}

static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
  req_count_read = 0;
  req_count_write = 0;
  bytes_read = 0;
  bytes_written = 0;
  printk("STATISTIC DMP RESET");
  return count;
}

static struct kobj_attribute sysfs_attribute =
    __ATTR(dmp_sysfs, 0665, sysfs_show, sysfs_store);

/*-------------------------------------------Module Functions ---------------------------------*/

static int init_dmp(void)
{
  int result;
  int sysfs_file_error = 0;
  result = dm_register_target(&dmp);
  if (result < 0)
    printk(KERN_CRIT "\n Error in registering target \n");

  dmp_sysfs = kobject_create_and_add("dmp_info", kernel_kobj);
  if (dmp_sysfs == NULL)
  {
    printk(KERN_CRIT "\n Error in registering sysfs dmp info\n");
    return -ENOMEM;
  }
  sysfs_file_error = sysfs_create_file(dmp_sysfs, &sysfs_attribute.attr);
  if (sysfs_file_error)
  {
    printk(KERN_CRIT "\n Error in creating sysfs dmp file\n");
    return -ENOMEM;
  }
  return 0;
}

static void cleanup_dmp(void)
{
  dm_unregister_target(&dmp);
  kobject_put(dmp_sysfs);
}

module_init(init_dmp);
module_exit(cleanup_dmp);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("lubitelkvokk");