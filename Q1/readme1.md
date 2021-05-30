#Concurrency merge sort

The sharemem function creates a shared memory of size 'size'

The merge function for all the ways is same.Only difference is during the partition of arrays.

## Using Processes

THe starting of the array and its position i.e l and r are  passed as input to the function .we take `int mid = (l+r-1)/2` and call the function with positions as `(int l, int mid)` and `(int mid+1, int r)`.
After returning from those functions, we merge the subarrays `arr[l..mid]` and `arr[mid+1..r]` by traversing both of them left to right and selecting the smaller element of the both and putting them in the final array and updating the original array

## Using Threads

The position of the array and array itself is passed as struct to the function .Th position contains l and r.We take `int mid = (l+r)/2` and create two threads in which we call the function and pass the positions as `(int l, int mid)` in one thread and pass the positions as `(int mid+1, int r)` in the other thread.After returning from both the threads, we merge the subarrays `arr[l..mid]` and `arr[mid+1..r]` in the parent process by traversing both of them left to right and putting them i a temp array and later mefgrging it.

####Time taken for n = 1000 is
- Normal recursive function method - **0.010105 sec**(approx)
- Child process recursion method - **0.903860 sec**(approx)
- Thread recursion method - **0.279691 sec**(approx)
Hence we observe that Normal Recursive function method is **89.675462**(approx) times faster than Child process recursive method and **27.813** times faster than Thread recursive method.