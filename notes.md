# Structure du code et de l'implémentation

## Load image (a)
- Load the image with opencv (use YUV and not RGB)
- Output :Matrix of size (height, width) with 3 elements (Y,U,V)

# Similarity graph with crossing edges (b)
- Output : Matrix of size 16x16,16x16 and store similar neighboors in list with 1 if "same" color 0 else (1 if i and j are neighboors with "same" color)

## Resolving crossing edge (c)
- Check crossing : if $m_{i,j} = 1$, check whether $m_{i+1,j-1} = 1$
- Curves : Check covalence : sum along an axis
- Sparse :
- Island :
- Choose which one to delete
- Output : the same (with 1 changed to 0)

## Reshaping cells (Voronoi) (d)
- 

## Spline fit to visible edges (e)
- 

## Rendering the image (f)