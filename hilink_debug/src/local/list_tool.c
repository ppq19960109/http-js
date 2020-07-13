#include "list_tool.h"

dev_data_t *list_get_by_id(const char *id, struct list_head *head)
{
    dev_data_t *ptr;
    if (head == NULL)
    {
        return NULL;
    }
    list_for_each_entry(ptr, head, node)
    {
        if (strcmp(ptr->DeviceId, id) == 0)
        {
            return ptr;
        }
    }
    return NULL;
}

void list_del_dev(dev_data_t *ptr)
{
    list_del(&ptr->node);
    if (ptr->private != NULL)
    {
        free(ptr->private);
    }
    free(ptr);
}

int list_del_by_id(const char *id, struct list_head *head)
{

    dev_data_t *ptr = list_get_by_id(id, head);
    if (ptr == NULL)
    {
        return -1;
    }
    list_del_dev(ptr);
    return 0;
}

void list_del_all(struct list_head *head)
{
    dev_data_t *ptr, *next;
    if (head == NULL)
    {
        return;
    }
    list_for_each_entry_safe(ptr, next, head, node)
    {
        list_del_dev(ptr);
    }
}

void list_print_all(struct list_head *head)
{
    dev_data_t *ptr;
    if (head == NULL)
    {
        return;
    }

    int i = 0;
    list_for_each_entry(ptr, head, node)
    {
        log_info(" ---------------------entry num:%d\n", i++);
        log_info("GatewayId:%s\n", ptr->GatewayId);
        log_info("DeviceType:%s\n", ptr->DeviceType);
        log_info("DeviceId:%s\n", ptr->DeviceId);
        log_info("ModelId:%s\n", ptr->ModelId);
        log_info("Version:%s\n", ptr->Version);
        log_info("Secret:%s\n", ptr->Secret);
        log_info("Online:%s\n", ptr->Online);
        log_info("RegisterStatus:%s\n", ptr->RegisterStatus);
    }
}