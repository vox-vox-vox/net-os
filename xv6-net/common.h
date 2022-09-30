struct queue_entry {
    void *data; // 任意数据
    size_t size;
    struct queue_entry *next;
};

struct queue_head {
    struct queue_entry *next;
    struct queue_entry *tail;
    unsigned int num;
};
