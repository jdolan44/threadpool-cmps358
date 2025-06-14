threadpool: blocking_queue.c thread_pool.c pool_tester.c
	gcc -lpthread -o threadpool blocking_queue.c thread_pool.c pool_tester.c -g

queuetest: blocking_queue.c queue_tester.c
	gcc -lpthread -o queuetest blocking_queue.c queue_tester.c 