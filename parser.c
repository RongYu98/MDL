#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"
#include "stack.c"

/*======== void parse_file () ==========
Inputs:   char * filename 
          struct matrix * transform, 
          struct matrix * pm,
          screen s
Returns: 

Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single character that takes up a line
     Any command that requires arguments must have those arguments in the second line.
     The commands are as follows:
         line: add a line to the edge matrix - 
	    takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
	 circle: add a circle to the edge matrix - 
	    takes 3 arguments (cx, cy, r)
	 hermite: add a hermite curve to the edge matrix -
	    takes 8 arguments (x0, y0, x1, y1, x2, y2, x3, y3)
	 bezier: add a bezier curve to the edge matrix -
	    takes 8 arguments (x0, y0, x1, y1, x2, y2, x3, y3)
         sphere: add a sphere to the edge matrix - 
	    takes 3 arguemnts (cx, cy, r)
         torus: add a torus to the edge matrix - 
	    takes 4 arguemnts (cx, cy, r1, r2)
         box: add a rectangular prism to the edge matrix - 
	    takes 6 arguemnts (x, y, z, width, height, depth)
	 clear: clear the currnt edge matrix -
	    takes 0 arguments
	 ident: set the transform matrix to the identity matrix - 
	 scale: create a scale matrix, 
	    then multiply the transform matrix by the scale matrix - 
	    takes 3 arguments (sx, sy, sz)
	 translate: create a translation matrix, 
	    then multiply the transform matrix by the translation matrix - 
	    takes 3 arguments (tx, ty, tz)
	 xrotate: create an x-axis rotation matrix,
	    then multiply the transform matrix by the rotation matrix -
	    takes 1 argument (theta)
	 yrotate: create an y-axis rotation matrix,
	    then multiply the transform matrix by the rotation matrix -
	    takes 1 argument (theta)
	 zrotate: create an z-axis rotation matrix,
	    then multiply the transform matrix by the rotation matrix -
	    takes 1 argument (theta)
	 apply: apply the current transformation matrix to the 
	    edge matrix
	 display: draw the lines of the edge matrix to the screen
	    display the screen
	 save: draw the lines of the edge matrix to the screen
	    save the screen to a file -
	    takes 1 argument (file name)
	 quit: end parsing

See the file script for an example of the file format


IMPORTANT MATH NOTE:
the trig functions int math.h use radian mesure, but us normal
humans use degrees, so the file will contain degrees for rotations,
be sure to conver those degrees to radians (M_PI is the constant
for PI)
====================*/
void parse_file ( char * filename, 
                  struct matrix * transform, 
                  struct matrix * pm,
                  screen s) {

  FILE *f;
  char line[256];
  struct matrix * tmp;
  double angle;
  color g;

  struct stack * STACK = new_stack();
  
  g.red = 0;
  g.green = 255;
  g.blue = 0;
  
  clear_screen(s);

  if ( strcmp(filename, "stdin") == 0 ) 
    f = stdin;
  else
    f = fopen(filename, "r");
  
  while ( fgets(line, 255, f) != NULL ) {
    line[strlen(line)-1]='\0';
    //printf(":%s:\n",line);
    double x, y, z, x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4;
   
    
    if ( strncmp(line, "line", strlen(line)) == 0 ) {
      //      printf("LINE!\n");
      fgets(line, 255, f);
      //      printf("\t%s", line);
      //line[strlen(line)-1]='\0';
      sscanf(line, "%lf %lf %lf %lf %lf %lf", &x, &y, &z, &x1, &y1, &z1);
      add_edge(pm, x, y, z, x1, y1, z1);
      // printf( "%lf %lf %lf %lf %lf %lf\n", x, y, z, x1, y1, z1);
      matrix_mult( STACK->data[ STACK->top], pm);
      draw_lines( pm, s, g);
      pm->lastcol = 0;
    }
    else if ( strncmp(line, "circle", strlen(line)) == 0 ) {
      //printf("CIRCLE\n");
      fgets(line, 255, f);
      sscanf(line, "%lf %lf %lf", &x, &y, &z);
      add_circle(pm, x, y, z, 0.01);
      //printf( "%lf %lf %lf\n", x, y, z);
      matrix_mult( STACK->data[ STACK->top], pm);
      draw_lines( pm, s, g);
      pm->lastcol = 0;
    }    
    else if ( strncmp(line, "bezier", strlen(line)) == 0 ) {
      fgets(line, 255, f);
      sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf",
	     &x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4);
      add_curve(pm, x1, y1, x2, y2, x3, y3, x4, y4, 0.01, BEZIER_MODE );
      //printf( "%lf %lf %lf\n", x, y, z);
      matrix_mult( STACK->data[ STACK->top], pm);
      draw_lines( pm, s, g);
      pm->lastcol = 0;

    }    
    else if ( strncmp(line, "hermite", strlen(line)) == 0 ) {
      //printf("HERMITE\n");
      fgets(line, 255, f);
      sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf",
	     &x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4);
      add_curve(pm, x1, y1, x2, y2, x3, y3, x4, y4, 0.01, HERMITE_MODE );
      //printf( "%lf %lf %lf\n", x, y, z);
      matrix_mult( STACK->data[ STACK->top], pm);
      draw_lines( pm, s, g);
      pm->lastcol = 0;
    }
    
    else if ( strncmp(line, "box", strlen(line)) == 0 ) {
      fgets(line, 255, f);
      sscanf(line, "%lf %lf %lf %lf %lf %lf", &x, &y, &z, &x1, &y1, &z1);
      add_box(pm, x, y, z, x1, y1, z1);
      matrix_mult( STACK->data[ STACK->top ], pm);
      draw_polygons( pm, s, g );
      pm->lastcol = 0;
    }
    
    else if (strncmp(line, "sphere", strlen(line)) == 0 ) {
      fgets(line, 255, f);
      sscanf(line, "%lf %lf %lf", &x, &y, &z);
      add_sphere(pm, x, y, z, 10);
      matrix_mult( STACK->data[ STACK->top ], pm);
      draw_polygons( pm, s, g );
      pm->lastcol = 0;
    }
    else if (strncmp(line, "torus", strlen(line)) == 0 ) {
      fgets(line, 255, f);
      sscanf(line, "%lf %lf %lf %lf", &x, &y, &z, &z1);
      add_torus(pm, x, y, z, z1, 10);
      matrix_mult( STACK->data[ STACK->top ], pm);
      draw_polygons( pm, s, g );
      pm->lastcol = 0;
    }
    else if ( strncmp(line, "scale", strlen(line)) == 0 ) {
      fgets(line, 255, f);
      //line[strlen(line)-1]='\0';      
      sscanf(line, "%lf %lf %lf", &x, &y, &z);
      tmp = make_scale(x, y, z);
      //matrix_mult(tmp, transform);
      //print_matrix(transform);
      //matrix_mult( tmp, STACK->data[ STACK->top ] );
      matrix_mult( STACK->data[ STACK->top ], tmp );
      copy_matrix( tmp, STACK->data[ STACK->top ] );
    }
    else if ( strncmp(line, "translate", strlen(line)) == 0 ) {
      //printf("TRANSLATE\n");
      fgets(line, 255, f); 
      sscanf(line, "%lf %lf %lf", &x, &y, &z);
      tmp = make_translate(x, y, z);
      //matrix_mult(tmp, transform);
      //matrix_mult( tmp, STACK->data[ STACK->top ] );
      matrix_mult( STACK->data[ STACK->top ], tmp );
      copy_matrix( tmp, STACK->data[ STACK->top ] );
    }
    else if ( strncmp(line, "xrotate", strlen(line)) == 0 ) {
      //printf("ROTATE!\n");
      fgets(line, 255, f);
      sscanf(line, "%lf", &angle);
      angle = angle * (M_PI / 180);
      tmp = make_rotX( angle);
      //matrix_mult(tmp, transform);
      //matrix_mult( tmp, STACK->data[ STACK->top ] );
      matrix_mult( STACK->data[ STACK->top ], tmp );
      copy_matrix( tmp, STACK->data[ STACK->top ] );
    }
    else if ( strncmp(line, "yrotate", strlen(line)) == 0 ) {
      //printf("ROTATE!\n");
      fgets(line, 255, f);
      sscanf(line, "%lf", &angle);
      angle = angle * (M_PI / 180);
      tmp = make_rotY( angle);
      //matrix_mult(tmp, transform);
      //matrix_mult( tmp, STACK->data[ STACK->top ] );
      matrix_mult( STACK->data[ STACK->top ], tmp );
      copy_matrix( tmp, STACK->data[ STACK->top ] );
    }
    else if ( strncmp(line, "zrotate", strlen(line)) == 0 ) {
      //printf("ROTATE!\n");
      fgets(line, 255, f);
      sscanf(line, "%lf", &angle);
      angle = angle * (M_PI / 180);
      tmp = make_rotZ( angle);
      //matrix_mult(tmp, transform);
      //matrix_mult( tmp, STACK->data[ STACK->top ] );
      matrix_mult( STACK->data[ STACK->top ], tmp );
      copy_matrix( tmp, STACK->data[ STACK->top ] );
    }
    else if ( strncmp(line, "ident", strlen(line)) == 0 ) {
      ident(transform);
    }
    else if ( strncmp(line, "apply", strlen(line)) == 0 ) {
      //printf("APPLY!\n");
      //print_matrix( transform );
      //      print_matrix(pm);
      matrix_mult(transform, pm);
    }
    else if ( strncmp(line, "print", strlen(line)) == 0) {
      print_matrix( STACK->data[ STACK->top ] );
    }
    else if ( strncmp(line, "display", strlen(line)) == 0 ) {
      display(s);
    }
    else if ( strncmp(line, "save", strlen(line)) == 0 ) {
      fgets(line, 255, f);
      // line[strlen(line)-1] = '\0';
      //clear_screen(s);
      //draw_polygons(pm, s, g);
      save_extension(s, line);
    }
    else if ( strncmp(line, "clear", strlen(line)) == 0 ) {
      pm->lastcol = 0;
    }
    else if ( strncmp(line, "quit", strlen(line)) == 0 ) {
      return;
    }
    else if ( strncmp(line, "push", strlen(line)) == 0 ) {
      push( STACK ); //seg fault
      printf("Pushed\n");
    }
    else if ( strncmp(line, "pop", strlen(line)) == 0 ) {
      pop( STACK );
      printf("Popped\n");
    } 
    else if ( line[0] != '#' ) {
      printf("Invalid command\n");
    }
  }
  
  free_matrix(tmp);
  fclose(f);
  //printf("END PARSE\n");
}
//note, try what DW said, reverse the multiplcation
 
/*
  
  transform = ..... 

  STACK->data[ STACK->top ] = matrix_multi( transform, data[ STACK->top  ])


Activity Log:
4/14
Added "pop" and "push"
Applied that transform/rotate/scale to everything
    multi != mult....
    stack->data != stack
4/15
Changed the save and display
added in the extra for box/sphere/torus
      matrix_mult( STACK->data[ STACK->top ], pm);
      draw_polygons( pm, s, g );
      pm->lastcol = 0;
4/18

Did what DW suggested, reversed the multiplication of matrices
did the line/curve/circles
*/
