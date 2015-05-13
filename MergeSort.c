/* Merge sort in C */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

struct PageTable{
    char * Name;
    int * PageNumber;      //array of page number
    int * MemoryLocation;  //indices stored parallel to PageNumber
    int offset;
    clock_t PageTableUseTime;
};


// Function to Merge Arrays L and R into A.
// lefCount = number of elements in L
// rightCount = number of elements in R.
void Merge(struct PageTable **pt, struct PageTable **Lpt, int leftCount, struct PageTable **Rpt, int rightCount) {
    int i,j,k;
    
    /*printf("Merging: \n\tL: ");
    
    for (i = 0; i<leftCount; i++)
    {
        printf("%lu ", Lpt[i]->PageTableUseTime);
    }
    printf("\nR: ");
    for (i = 0; i<rightCount; i++)
    {
        printf("%lu ", Rpt[i]->PageTableUseTime);
    }
    printf("\n");*/
    
    // i - to mark the index of left aubarray (L)
    // j - to mark the index of right sub-raay (R)
    // k - to mark the index of merged subarray (A)
    i = 0; j = 0; k =0;
    
    while(i<leftCount && j< rightCount) {
        if(Lpt[i]->PageTableUseTime  > Rpt[j]->PageTableUseTime) pt[k++] = Lpt[i++];
        else pt[k++] = Rpt[j++];
    }
    while(i < leftCount) pt[k++] = Lpt[i++];
    while(j < rightCount) pt[k++] = Rpt[j++];
}

// Recursive function to sort an array of integers.
/*void MergeSort(int *A,int n) {
    int mid,i, *L, *R;
    if(n < 2) return; // base condition. If the array has less than two element, do nothing.
    
    mid = n/2;  // find the mid index.
    
    // create left and right subarrays
    // mid elements (from index 0 till mid-1) should be part of left sub-array
    // and (n-mid) elements (from mid to n-1) will be part of right sub-array
    L = (int*)malloc(mid*sizeof(int));
    R = (int*)malloc((n- mid)*sizeof(int));
    
    for(i = 0;i<mid;i++) L[i] = A[i]; // creating left subarray
    for(i = mid;i<n;i++) R[i-mid] = A[i]; // creating right subarray
    
    MergeSort(L,mid);  // sorting the left subarray
    MergeSort(R,n-mid);  // sorting the right subarray
    Merge(A,L,mid,R,n-mid);  // Merging L and R into A as sorted list.
    free(L);
    free(R);
}*/

void MergeSort(struct PageTable **pt, int n) {
    int mid,i;
    struct PageTable **L = malloc(sizeof(struct PageTable*)*((n/2) + 1));
    struct PageTable **R = malloc(sizeof(struct PageTable*)*((n/2) + 1));
    if(n < 2) return; // base condition. If the array has less than two element, do nothing.
    
    mid = n/2;  // find the mid index.
    
    // create left and right subarrays
    // mid elements (from index 0 till mid-1) should be part of left sub-array
    // and (n-mid) elements (from mid to n-1) will be part of right sub-array
    L = malloc(mid*sizeof(struct PageTable));
    R = malloc((n- mid)*sizeof(struct PageTable));
    
    for(i = 0;i<mid;i++) L[i] = pt[i]; // creating left subarray
    for(i = mid;i<n;i++) R[i-mid] = pt[i]; // creptting right subarray
    
    MergeSort(L,mid);  // sorting the left subarray
    MergeSort(R,n-mid);  // sorting the right subarray
    Merge(pt,L,mid,R,n-mid);  // Merging L and R into A as sorted list.
    free(L);
    free(R);
}


/*int main() {
    // Code to test the MergeSort function.
    
    struct PageTable **A = malloc(sizeof(struct PageTable*)*4); // creating an array of integers.
    struct PageTable *a = malloc(sizeof(struct PageTable));
    a->Name = "TableA";
    a->PageTableUseTime = clock();
    
    printf("Time a: %lu\n", a->PageTableUseTime);
    
    sleep(2);
    
    struct PageTable *b = malloc(sizeof(struct PageTable));
    b->Name = "TableB";
    b->PageTableUseTime = clock();
    
    printf("Time b: %lu\n", b->PageTableUseTime);
    
    sleep(3);
    
    struct PageTable *c = malloc(sizeof(struct PageTable));
    c->Name = "TableC";
    c->PageTableUseTime = clock();
    
    printf("Time c: %lu\n", c->PageTableUseTime);
    
    
    sleep(1);
    struct PageTable *d = malloc(sizeof(struct PageTable));
    d->Name = "TableD";
    d->PageTableUseTime = clock();
    
    
    printf("Time d: %lu\n", d->PageTableUseTime);
    
    A[0] = c;
    A[1] = a;
    A[2] = d;
    A[3] = b;
    
    
    int i,numberOfElements;
 
    numberOfElements = 4;
    
    // Calling merge sort to sort the array.
    MergeSort(A,numberOfElements);
    
    //printing all elements in the array once its sorted.
    for(i = 0;i < numberOfElements;i++) printf("%lu ",A[i]->PageTableUseTime);
    printf("\n");
    
    
    return 0;
}*/