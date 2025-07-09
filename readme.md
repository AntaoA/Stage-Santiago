Internship with Prof. Gonzalo Navarro in Santiago (Chile) from 09-06-25 to 01-08-25.

------
Wednesday 11th June

001 - Dynamic programming for approximate matching
    Calculation of all the matrix (some upgrades are possibles)
    Find all aproximate matching with less than k errors and print the errors

002 - Approximate matching using suffix tree

003 - Approximate matching using fragments
    redundancy?
    recurcivity?

004 - Approximate matching using an hybrid version




Paper - A Fast and Small Subsampled R-index

    sr-index:   time    <=>     r-index
                space   <=>     rlfm-index


    SA : suffix array
    BWT[i] = T[SA[i] - 1]
    LSA[p] = SA[i] - 1 when i is the last letter of the run p       if SA[i] = 1 then LSA[p] = last letter ($) id
    FT : bitvector of first letter of each runs
    pred(FT, j) : first 1 equal or before j in FT
    rank(FT, k) : number of 1 in FT[0..k]   
    select(FT, k) : k^th 1 of FT

    pred(FT,j) = select(FT, rank(FT,j))

    MapFL(p) = number of the previous run before SA[i + 1] where i is the p^th bit marked

    sr-index:
        s parameter
        we sort LSA and subsample it regarding s
        we repercute the subsample on FT and on MapFL

	findToehold(P) -> find the position of an occurance of P in T
	count(P), return sp, ep such that ep-sp+1 = number of occurance of P in T, and findToehold(P)
	locate(P) return the position of all occurance of P in T
	
	phi(i) return the text position of the prefix just before the prefix starting in the position i	
	
005 - Creation of complete sr-index lib

006 - AppMat on SR-index
    - Modification of count algorithm
        We check all posibilities and we maintain an error's counter
        
        ? Possibility of keep in memory the stack ? (for avoiding to recalculate several time)
        ? Possibility of regrouping stack regarding the error's counter ?

    FindToeHold and Locate are not changed.