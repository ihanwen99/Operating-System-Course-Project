# Homework for Chapter6

### 6.8

> Race conditions are possible in many computer systems. Consider an online auction system where the current highest bid for each item must be maintained. A person who wishes to bid on an item calls the `bid(amount) function`, which compares the amount being bid to the current highest bid. If the amount exceeds the current highest bid, the
> highest bid is set to the new amount. This is illustrated below:
>
> ```c++
> void bid(double amount){
> 	if(amount > highestBid)
> 		highestBid = amount;
> }
> ```
>
> Describe how a race condition is possible in this situation and what might be done to prevent the race condition from occurring.

***ANSWER***

Assume the current highestBid is \$100, and A and B call the bid function with the amount of \$150 and \$200. Obviously the correct value of highestBid is \$200. Since these two transactions will be serialized, the local value of highestBid of A is \$150, but before he can commit the transaction, the `bid($200)` operation takes place and updates the shared value of highestBid to \$200. We then switch back to A and the value of the shared value of highestBid is set to \$150, which obviously an incorrect value.

### 6.13

> The first known correct software solution to the critical-section problem for two processes was developed by Dekker. The two processes, P0 and P1, share the following variables:
>
> ```c++
> boolean flag[2]; /* initially false */
> int turn;
> ```
>
> The structure of process $P_i$ ( i == 0 or 1) is shown in Figure 6.18. The other process is $P_j $(j == 1  or  0). Prove that the algorithm satisfies all three requirements for the critical-section problem.

***Answer***

```c++
flag[i] = True;
turn = j;
while (flag[j] && turn==j)；
 /* critical section */
flag[i] = Ture;
/* remainder section*/
```

> `Flag[i]=Ture` [in my mind] represents whether it is ready to execute the critical section.
>
> `turn=j`  [in my mind] represents now we give the opportunity to j, to check whether the other process(j) is in the critical section(have not changed the value of flag[j]).

1. **Mutual Exclusion**

   The waiting process can only enter its critical section when ` flag[j]=false` or `turn =i`. In this case, by the initiation of $P_i$, it could enter the critical section until `flag[j] = false`(which means $P_j$ has exited the critical section).

2. **Progress**

   The process(supposed to i) which wants to enter the critical section again before other process, it needs to execute the code again, where it sets `turn==j`, which gives other process(j) an opportunity to enter the critical section.

3. **Bounding Waiting**

   Just like I shown before, if it wants to enter the critical section, it sets the value of **flag**, and then enters the critical section. And it will change the value of **flag** upon exiting its critical section, which give other process an opportunity to enter the critical section.

### 6.21

> A multithreaded web server wishes to keep track of the number of requests it services (known as hits). Consider the two following strategies to prevent a race condition on the variable hits. The first strategy is to use a basic mutex lock when updating hits:
>
> ```c++
> int hits;
> mutex_lock hit_lock;
> 
> hit_lock.acquire();
> hits++;
> hit_lock.release();
> ```
>
> A second strategy is to use an atomic integer:
>
> ```c++
> atomic_t hits;
> atomic_inc(&hits);
> ```
>
> Explain which of these two strategies is more efficient.

***Answer***

The second strategy is better.

1. By using atomic integer and the atomic increase operation, there will be no race condition on hits. And this can be accomplished with no kernel intervention, which leads it more efficient.
2. On the other hand, the lock operation is too strong to be used in just increasing a integer. Locking generally requires a system call and possibly putting the other process in to sleep, in this process, it will increase the consumption of the system by the context switches.

