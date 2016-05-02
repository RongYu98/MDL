/*========== my_main.c ==========

  This is the only file you need to modify in order
  to get a working mdl project (for now).

  my_main.c will serve as the interpreter for mdl.
  When an mdl script goes through a lexer and parser, 
  the resulting operations will be in the array op[].

  Your job is to go through each entry in op and perform
  the required action from the list below:

  push: push a new origin matrix onto the origin stack
  pop: remove the top matrix on the origin stack

  move/scale/rotate: create a transformation matrix 
                     based on the provided values, then 
		     multiply the current top of the
		     origins stack by it.

  box/sphere/torus: create a solid object based on the
                    provided values. Store that in a 
		    temporary matrix, multiply it by the
		    current top of the origins stack, then
		    call draw_polygons.

  line: create a line based on the provided values. Store 
        that in a temporary matrix, multiply it by the
	current top of the origins stack, then call draw_lines.

  save: call save_extension with the provided filename

  display: view the image live
  
  jdyrlandweaver
  =========================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "parser.h"
#include "symtab.h"
#include "y.tab.h"

#include "misc_headers.h"
#include "matrix.h"
#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "stack.h"

void my_main( int polygons ) {

  int i;
  double step;
  double xval, yval, zval, angle;
  struct matrix *transform;
  struct matrix *pm;
  struct matrix *tmp;
  struct stack *s;
  screen t;
  color g;

  g.red = 0;
  g.green = 255;
  g.blue = 0;
  
   
  s = new_stack();
  pm = new_matrix(4,1000);
  //tmp = new_matrix(4, 1000);
  tmp = new_matrix(4,4);
  ident(tmp);
  clear_screen( t );
  
  for (i=0;i<lastop;i++) {  

    switch (op[i].opcode) {

    case PUSH:
      printf("attempting to push\n");
      push( s );
      printf("pushed\n");
      break;
      
    case POP:
      printf("Attempting to pop\n");
      pop( s );
      printf("pop\n");
      break;

    case MOVE:
      printf("going to move\n");
      //printf('\s',op[i].opcode);
      xval = op[i].op.move.d[0];
      yval = op[i].op.move.d[1];
      zval = op[i].op.move.d[2];
      
      tmp = make_translate(xval, yval, zval);
      matrix_mult( s->data[ s->top ], tmp );
      copy_matrix( tmp, s->data[ s->top ] );
      printf("Moved\n");
      ident(tmp);
      break;

    case ROTATE: //line 625 of mdl.y
      //printf('\s',op[i].opcode);

      printf("going to rotate\n");
      angle = op[lastop].op.rotate.degrees;

      if (op[i].op.rotate.axis == 0){
        tmp = make_rotX( angle * (M_PI/180) );
      } else if (op[i].op.rotate.axis == 1){
        tmp = make_rotY( angle * (M_PI/180) );
      } else if (op[i].op.rotate.axis == 2){
        tmp = make_rotZ( angle * (M_PI/180) );
      }

      matrix_mult( s->data[ s->top ], tmp );
      copy_matrix( tmp, s->data[ s->top ] );
      printf("rotated\n");
      ident(tmp);
      break;

    case SCALE: //line 589 of mdl.y
      //printf('\s',op[i].opcode);

      printf("About to scale\n");
      xval = op[i].op.scale.d[0];
      yval = op[i].op.scale.d[1];
      zval = op[i].op.scale.d[2];
      tmp = make_scale(xval, yval, zval);

      matrix_mult( s->data[ s->top ], tmp );
      copy_matrix( tmp, s->data[ s->top ] );
      printf("Scaled\n");
      ident(tmp);      
      break;

    case BOX:
      //printf('\s',op[i].opcode);
      printf("making box\n");
      xval = op[i].op.box.d0[0];
      yval = op[i].op.box.d0[1];
      zval = op[i].op.box.d0[2];
      double dx = op[i].op.box.d1[0];
      double dy = op[i].op.box.d1[1];
      double dz = op[i].op.box.d1[2];

      add_box(pm, xval, yval, zval, dx, dy, dz);
      matrix_mult( s->data[ s->top ], pm);
      draw_polygons( pm, t, g );
      pm->lastcol = 0;
      printf("box made\n");
      break;

    case SPHERE:
      //printf('\s',op[i].opcode);

      printf("making sphere\n");
      xval = op[i].op.sphere.d[0];
      yval = op[i].op.sphere.d[1];
      zval = op[i].op.sphere.d[2];
      double r = op[i].op.sphere.r;

      add_sphere(pm, xval, yval, zval, r, 10);
      matrix_mult( s->data[ s->top ], pm);
      draw_polygons( pm, t, g );
      pm->lastcol = 0;
      printf("s made\n");
      break;
  
    case TORUS:
      //printf('\s',op[i].opcode);
      printf("m torus\n");
      xval = op[i].op.torus.d[0];
      yval = op[i].op.torus.d[1];
      zval = op[i].op.torus.d[2];
      double r1 = op[i].op.torus.r0;
      double r2 = op[i].op.torus.r1;

      add_torus(pm, xval, yval, zval, r1, r2, 10);
      matrix_mult( s->data[ s->top ], pm);
      draw_polygons( pm, t, g );
      pm->lastcol = 0;
      printf("torus done\n");
      break;
      
    case LINE:
      //printf('\s',op[i].opcode);

      printf("making line\n");
      xval = op[i].op.line.p0[0];
      yval = op[i].op.line.p0[1];
      zval = op[i].op.line.p0[2];
      double x2 = op[i].op.line.p1[0];
      double y2 = op[i].op.line.p1[1];
      double z2 = op[i].op.line.p1[2];

      printf("%d, %d, %d, %d, %d, %d", xval, yval, zval, dx, dy, dz);
      
      add_edge(pm, xval, yval, zval, x2, y2, z2);
      //print_matrix(pm);
      matrix_mult( s->data[ s->top], pm);
      draw_lines( pm, t, g);
      pm->lastcol = 0;
      printf("line made\n");
      break;

    case SAVE:
      printf("Attempting to save\n");
      //printf('\s',op[i].opcode);
      //printf(op[i].op.save.p);
      printf("\n");
      save_extension(t, op[i].op.save.p);
      printf("Saved\n");
      break;

    case DISPLAY:
      //printf('\s',op[i].opcode);
      display(t);
      printf("Displayed\n");
      break;

    default:
      //printf('\s',op[i].opcode);
      printf("something else..\n");
    }
 }
}
