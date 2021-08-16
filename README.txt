The program reads from standard input, displays a visual simulation of fortune's algorithm and upon closing
the window, writes a description of the diagram to the standard output.

Random input:
	The generate_input target is provided for convenience
	This program reads a single integer N from standard input
	It writes N random vertices to output
	The output of this program can be used as input for demo

Keyboard controls for simulation:
	p: Play/Pause simulation
	0: Set simulation progress to beginning
	1: Set simulation progress to ended
	w: Wireframe mode (hides cell faces)
	r: Reset rotation
	z: Rotate around Z axis
	x: Rotate around X axis
	c: Rotate around Y axis
	RSHIFT + z: Reverse rotate around Z axis
	RSHIFT + x: Reverse rotate around X axis
	RSHIFT + c: Reverse rotate around Y axis

Input:
	At least two site vertices must be provided as input
	Input must not contain any duplicate vertices

Format:
	The program reads from the standard input until EOF is reached. Each line contains three space
	separated real numbers, the coordinate position value of each vertex along the X, Y and Z axes. Any
	duplicate vertices will be merged and treated as a single vertex.

	A description of the generated Voronoi diagram is written to the standard output.
	The output begins with a single integer S (number of unique vertices). S lines follow for each unique
	vertex similar to input file format. The next line contains a single integer V (number of unique vertices
	describing the resultant voronoi cells) by itself. The next V lines describe the position of each vertex in a
	format similar to the site vertices. From the next line onwards, cell faces are described. The following is
	repeated S times in an order corresponding to the S site vertices: A single integer F is given on a line by
	itself corresponding to the number of faces describing that particular cell. The next F lines contain space
	separated integers. The first integer in this line is VF (the number of vertices that make up the polygonal
	face). VF more integers are given on the same line corresponding to the index of (V) vertices provided
	earlier.

