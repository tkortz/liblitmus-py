import liblitmus

def fib(n):
    if n == 0:
        return 0

    if n == 1 or n == 2:
        return 1

    return fib(n-1) + fib(n-2)

liblitmus.call_set_rt_task_param(50, 1000, 100, 0, False)
print("\nFinished setting rt params.\n")

liblitmus.call_init_litmus()
print("\nCalled init_litmus.\n")

liblitmus.set_task_mode_litmusrt()
print("\nNow a real-time task.\n")

liblitmus.call_wait_for_ts_release()

# Do some work
for i in range(32):
    print("fib({0}) is {1}".format(i, fib(i)))

    liblitmus.call_sleep_next_period()

liblitmus.set_task_mode_background()
print("\nNow a background task again.\n")
