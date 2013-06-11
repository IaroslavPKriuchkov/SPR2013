/* FILE NAME: MEGACOW.C
 * PROGRAMMER: IK1
 * DATE: 06.06.2013
 * PURPOSE: Cow animation module.
 */

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

#include "anim.h"

/* w, h of window */
extern INT w, h;

typedef struct
{
  IK1_UNIT_BASE_FUNCS;
  FLT X, Y, Z; 
  INT RandShift;
  FLT RandScale;
  INT Who; 
} COW;

static UINT CowProg;

/* Vertex array */
VEC *Vertexes;
static IK1_POINT *VertexesProj;
INT NumOfVertexes;

/* Facet array */
INT (*Facets)[3];
INT NumOfFacets;

/* Load cow function */
VOID LoadCow( VOID )
{
  FILE *F;
  INT fn = 0, vn = 0;
  static CHAR Buf[1000];

  if ((F = fopen("cow_new1.object", "rt")) == NULL)
    return;

  while (fgets(Buf, sizeof(Buf), F) != NULL)
    if (Buf[0] == 'v' && Buf[1] == ' ')
      vn++;
    else if (Buf[0] == 'f' && Buf[1] == ' ')
      fn++;

  if ((Vertexes = malloc(2 * sizeof(VEC) * vn)) == NULL)
  {
    fclose(F);
    return;
  }
  if ((Facets = malloc(sizeof(INT [3]) * fn)) == NULL)
  {
    free(Vertexes);
    fclose(F);
    return;
  }
  NumOfVertexes = vn;
  NumOfFacets = fn;
  VertexesProj = (IK1_POINT *)(Vertexes + NumOfVertexes);

  vn = 0;
  fn = 0;
  rewind(F);
  while (fgets(Buf, sizeof(Buf), F) != NULL)
    if (Buf[0] == 'v' && Buf[1] == ' ')
    {
      FLT x, y, z;

      sscanf(Buf + 2, "%f %f %f", &x, &y, &z);
      Vertexes[vn].X = x;
      Vertexes[vn].Y = y;
      Vertexes[vn].Z = z;
      vn++;
    }
    else if (Buf[0] == 'f' && Buf[1] == ' ') //&& Buf[2] == ' ')
    {
      INT n1, n2, n3;

      sscanf(Buf + 2, "%d%d%d", &n1, &n2, &n3);
      Facets[fn][0] = n1 - 1;
      Facets[fn][1] = n2 - 1;
      Facets[fn][2] = n3 - 1;
      fn++;
    }

  fclose(F);
} /* End of 'LoadCow' function */

static VOID CowRender( COW *Unit, ik1ANIM *Ani )
{
  /*
  int j;
  static UINT CowProg;


  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
  glColor3d(1, 1, 0);
  
  glRectd(-1, -1, 0, 1);
  glColor3d(1, 0, 0);
  glRectd(0, 0, 0.5, 0.5);
  glColor3d(0, 1, 0);
  glRectd(0.25, 0.25, 0.75, 0.75);
  glColor3d(0, 0, 1);
  glRectd(0.5, 0.5, 1, 1);
  

  //j = glGetUniformLocation(CowProg, "DrawColor");
  //glUniform4d(j, 1, 0, 0, 1);

  glColor3d(1, 1, 0);
  glBegin(GL_TRIANGLES);
    glVertex3d(0, 0, 0);
    glVertex3d(1, 0, 0);
    glVertex3d(0, 1, 0);
  glEnd();
  */
  CHAR Buf[100];
  FLT x = 0, y = 0;
  FLT t = (FLT)clock() / CLOCKS_PER_SEC;
  INT
    i, j;
  MATR W =
  {
  {
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
  }
}, V, P, m;

  UINT Matrix, Time;
/* switch (Ani->Jpov)
  {
  case 1:
    Unit->Y -= 0.1;
    break;
  case 2:
    ///Unit->X += sqrt(2) * 0.1;
   /// Unit->Y -= sqrt(2) * 0.1;
    break;
  case 3:
    Unit->X -= t;  
    break;
  case 4:
    ///Unit->X += sqrt(2) * 0.1;
   /// Unit->Y += sqrt(2) * 0.1;
    break;
  case 5:
     Unit->Y += 0.1;
    break;
  case 6:
   ///Unit->X -= sqrt(2) * 0.1;
   ///Unit->Y += sqrt(2) * 0.1;   
    break;
  case 7:
    Unit->X += 0.1;
    break;
  case 8:
    Unit->Y -= sqrt(2) * 0.1;
    Unit->X -= sqrt(2) * 0.1;  
    break;
  default:
    break;
  };                           */
  
  W = MatrMulMatr(MatrMulMatr(MatrScale(3, 3, 3), MatrRotate(t * 5, 1, 2, 3)), MatrTranslate( Ani->Jx * t, Ani->Jy * t, Ani->Jr * t));   
  V = MatrViewLookAt(VecSet(0, 0, 20), VecSet(0, 0, 0), VecSet(0, -1, 0)); 
  P = MatrProject(-0.5, 0.5, 0.5, -0.5, 0.1, 100); 

  m = AllMatrixCount(W, V, P);

  /*sprintf(Buf," FPS : %lf", Ani->FPS);
  SetWindowText(Ani->hWnd, Buf);
  */
  CowProg = ShadProgInit("a.vert", "a.frag");

  Matrix = glGetUniformLocation(CowProg, "Matr");
  Time = glGetUniformLocation(CowProg, "Time"); 
   
  if( Time != -1)
    glUniform1f(Time, t);

  glUseProgram(CowProg);  

  for (i = 0; i < NumOfFacets; i++)
  {
    VEC p[3];

    for (j = 0; j < 3; j++)
    {
      
       p[j].X = Vertexes[Facets[i][j]].X;
       p[j].Y = Vertexes[Facets[i][j]].Y;
       p[j].Z = Vertexes[Facets[i][j]].Z;
    }
    glUniformMatrix4fv(Matrix, 1, FALSE, m.A[0]);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_TRIANGLES);
      glVertex3fv(&p[0].X);
      glVertex3fv(&p[1].X);
      glVertex3fv(&p[2].X);
    glEnd();
  } 

  glUseProgram(0); 
}

ik1UNIT *CowCreate( FLT X, FLT Y, FLT Z)
{
  COW *Unit;

  if ((Unit = (COW *)IK1_UnitCreate(sizeof(COW))) == NULL)
    return NULL;

  Unit->Render = (ik1UNIT_RENDER)CowRender;
  Unit->X = X;
  Unit->Y = Y;
  Unit->Z = Z;
  Unit->Who = 0;
  Unit->RandShift = rand() % 1000;
  Unit->RandScale = 0.75 + 5.5 * rand() / (FLT)RAND_MAX;
  return (ik1UNIT *)Unit;
} /* End of 'CowCreate' function */

/* END OF 'MEGACOW.C' FILE */