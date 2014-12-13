=======
README
=======


Controls:

to move a shape on the current film plane, click and drag

to combine electric and water drag an electric into a water shape that has no electric

to remove the electric, either hold Ctrl (Cmd for macs) and drag the electric out, or delete the water

to delete a shape, hold the d key and click on a shape (there must always be at least 1 shape of each type, and there can be no more than 5 of each shape type)
	Be wary a multiple water shapes can significantly slow done the program (covered in more detail later)

to move a shape on the Z plane, hold shift, then click and drag up or down.

to add a water shape, type the w key (there can be a maximum of 5 water shapes)

to add a lightning shape, type the l key (there can be a maximum of 5 lightning shapes)

to start/stop camera rotation, type R, (type again to stop rotation)



*******
DESIGN
*******

The features we implemented were as follows:


Piecewise Bezier Surfaces:

	Used to smooth out the frequency input data into a smooth surface on the sphere.


Collision Detection:

	Used to determine when lightning/water combines, or when to translate objects away from collisions. (We do not highlight colliding objects, but rather the currently selected object is the aggressor in all collisions, and if you hold down Ctrl/Cmd (to translate out lightning shapes), collisions are ignored.


Fractal Terrain:

	We use a variation on the midpoint algorithm in order to generate fractal bolts of lightning for the electricity objects.


Skybox:
	
	We implemented a skybox in order to give the water spheres a more realistic feel


Geometry Box:


—————
BUGS
—————

	Depending on which version of the audio program you are running along with our project, you may occasionally see a water sphere render strangely. Do not be alarmed, simply restart the program and it should render cleanly.