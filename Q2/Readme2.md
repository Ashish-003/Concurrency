#Question 2
# Vaccination

##Overview
The program simulates the process of production, distribution and testing of vaccines on students . The companies produces the vaccines which are then transported to the vaccination zones .Students are allocated a vaccination zone where they are injected vaccines. If they passed the antibody test , then they are sent to college else they have to get vaccinated again . If any student fails the test 3 times , he is sent back home.

##Implementation

Three structs ar declared which are passed during the inialization of thread functions.

INformation is stored in wither global variables or global arrays of the considered structs.
o represents the number of students pending to be vaccinated correctly or sent of home

## code is divided into 4 parts
-make vaccines
-distribute them
-vacc zones
-students

###make vaccines
 - Each Pharmaceutical Company is treated as a separate thread. 
 - The Company takes a random amount of time to prepare the vaccines .
 - It produces r(also random) batches of vaccines.

###distribute vaccines
 - It checks if any of the zones has ran out of vaccines by iterating over the struct array of the zones (i.e `zones[i].status`).
 -The mutex named `mutex` ensures that no two companies provide vaccines to the same zone at the same time. 
 - After distributing all the vaccines to the zones , the company threads busy -waits for its vaccines to get exhausted on students in vaccination zones.
 - After the vaccines are exhausted the company again starts manufacturing vaccines by going back to the make vaccines function

###Students
- Each student is also treated as a separate thread.
-The number of times student comes for test is stored in students[].try
- After arrival students signals the vaccination zones through the `student[].status` in struct .The student then busy waits on this
- If the student tests positve for antibodies then he is sent to college else he has to join the queue.

###Vaccination Zones
-  Each Vaccination Zone is treated as separate thread 
-  Initially in the main function zones[i].status struct array is all set to 0 and the zone thread busy waits for the companies to deliever the vaccines.
-  After the zones receive the vaccines ,it creates k slots. The value of k is set so that it does not exceed the students waiting and the vaccines remaining in the zones.
 -  The mutex named `mutex2` ensures that the same students is not allocated to two different zone at the same time. It also ensures that the value of `waiting_stu` do not change while zone is allocating students. 
 -  After allocating k students the zone , the zone enters the vaccination zones and all students are vaccinated and afterwards are tested for antibodies.
- If the zone ran out of vaccines it changes its signal array and thus informing companies that it needs vaccines and then waits for vaccines to arrive.

##Assumptions
- The values of probabilty are out of 100 in integer.
- Vaccination not possible if either n or m or o is equal to zero.


