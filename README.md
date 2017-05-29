# BathroomStalls
Google Code Jam 2017

## Algorithm
    
     Definitions:
     Group =    A number of consecutive free stalls, in the beginning there is only one group.
     Layer =    A new layer is started when all groups of the previous layer are split up.
                Each layer holds therefore 2^(layer - 1) customers
                e.g.    1st layer: 1 customer
                        2nd layer: 2 customers
                        3rd layer: 4 customers
                        .
                        nth layer: 2^(n-1) customers
    
     With the above definitions it can be calculated how many layers are necessary.
         (Eq. 1) lastLayer = ceil(log(numberCustomers)/log(2))
    
     To calculate the size of the group, the last customer will be assigned to, the size of the
     groups in the last layer must be calculated. To do so, we need the following:
     - number of stalls in the last layer
         (Eq. 2) custPrevLayers = 2^(lastLayer - 1) - 1
         (Eq. 3) stallsLastLayer = totalStalls - custPrevLayers
     - the number of groups
         (Eq. 4) nbrGroupsLastLayer = 2^(lastLayer - 1)
     - and the number of customers in the last layer
         (Eq. 5) custLastLayer = totalCustomers - custPrevLayers
    
     Thus, the average group size is
         (Eq. 6) avgGroupSizeLastLayer = stallsLastLayer / nbrGroupsLastLayer
    
     Since avgGroupSizeLastLayer will in most cases not be an integer, we end up with two different
     group sizes:
         (Eq. 7) largeGroupSize = ceil(avgGroupSizeLastLayer)
         (Eq. 8) smallGroupSize = largeGroupSize - 1
    
     The last step is to figure out how many large groups are present in the last layer. This can
     be calculated by means of (Eq. 9) and (Eq. 10):
         (Eq. 9)  stallsLastLayer    = largeGroupSize * nbrLargeGroups + smallGroupSize * nbrSmallGroups
         (Eq. 10) nbrGroupsLastLayer = nbrLargeGroups + nbrSmallGroups
    
         Solving (Eq. 9) for nbrSmallGroups and inserting it into (Eq. 10) leaves us with
         (Eq. 11) nbrLargeGroups = (stallsLastLayer - (nbrGroupsLastLayer * smallGroupSize))/(largeGroupSize - smallGroupSize);
    
     If custLastLayer is smaller than the nbrLargeGroups min and max is to be calculated
     with the largeGroupSize, otherwise with the smallGroupSize.