#ifndef VERTEX_ARRAY_HOLDER_H
#define VERTEX_ARRAY_HOLDER_H
#include "ofMain.h"
class vertex_array_holder
{
	int count; //counter
	int glmode; //drawing mode in opengl
	bool jumping;
public:
	const static int MAX_VERT=512;
	float vert[MAX_VERT*2]; //because it holds 2d vectors
	float color[MAX_VERT*4]; //RGBA
	
	vertex_array_holder()
	{
		count = 0;
		glmode = GL_TRIANGLES;
		jumping = false;
	}
	//--------------------------------------------------------------
	void set_gl_draw_mode( int gl_draw_mode){
		glmode = gl_draw_mode;
	}
	//--------------------------------------------------------------
	int get_count() const{
		return count;
	}
	//--------------------------------------------------------------
	void clear(){
		count = 0;
	}
    //--------------------------------------------------------------
	void move( int a, int b){ //move b into a
		vert[a*2]   = vert[b*2];
		vert[a*2+1] = vert[b*2+1];
		
		color[a*4]  = color[b*4];
		color[a*4+1]= color[b*4+1];
		color[a*4+2]= color[b*4+2];
		color[a*4+3]= color[b*4+3];
	}
    //--------------------------------------------------------------
	void replace( int a, ofVec2f P, ofFloatColor C)
	{
		vert[a*2]   = P.x;
		vert[a*2+1] = P.y;
		
		color[a*4]  = C.r;
		color[a*4+1]= C.g;
		color[a*4+2]= C.b;
		color[a*4+3]= C.a;
	}
	//--------------------------------------------------------------
	int draw_and_flush()
	{
		int& i = count;
		draw();
		switch( glmode)
		{
			case GL_POINTS:
				i=0;
			break;
			
			case GL_LINES:
				if ( i%2 == 0) {
					i=0;
				} else {
					goto copy_the_last_point;
				}
			break;
			
			case GL_TRIANGLES:
				if ( i%3 == 0) {
					i=0;
				} else if ( i%3 == 1) {
					goto copy_the_last_point;
				} else {
					goto copy_the_last_2_points;
				}
			break;
			
			case GL_LINE_STRIP: case GL_LINE_LOOP:
			//for line loop it is not correct
			copy_the_last_point:
				move(0,MAX_VERT-1);
				i=1;
			break;
			
			case GL_TRIANGLE_STRIP:
			copy_the_last_2_points:
				move(0,MAX_VERT-2);
				move(1,MAX_VERT-1);
				i=2;
			break;
			
			case GL_TRIANGLE_FAN:
				//retain the first point,
				// and copy the last point
				move(1,MAX_VERT-1);
				i=2;
			break;
			
			case GL_QUAD_STRIP:
			case GL_QUADS:
			case GL_POLYGON:
			//let it be and I cannot help
				i=0;
			break;
		}
		if ( i == MAX_VERT) //as a double check
			i=0;
	}
	//--------------------------------------------------------------
	int push( const ofVec2f& P, const ofFloatColor& cc, bool transparent=false)
	{
		int& i = count;
		int cur;
		
		if ( i+1 == MAX_VERT)
		{	//when the internal array is full
			draw_and_flush();
		}
		
		cur = i;
		vert[i*2]  = P.x;
		vert[i*2+1]= P.y;
		
		color[i*4]  = cc.r;
		color[i*4+1]= cc.g;
		color[i*4+2]= cc.b;
		if ( transparent==true)
			color[i*4+3]= 0.0f;
		else
			color[i*4+3]= cc.a;
		
		i++;
		
		if ( jumping)
		{
			jumping=false;
			repeat_last_push();
		}
		
		return cur;
	}
	//--------------------------------------------------------------
	void push3( const ofVec2f& P1, const ofVec2f& P2, const ofVec2f& P3,
			const ofFloatColor& C1, const ofFloatColor& C2, const ofFloatColor& C3,
			bool trans1=0, bool trans2=0, bool trans3=0)
	{
		push( P1,C1,trans1);
		push( P2,C2,trans2);
		push( P3,C3,trans3);
	}
	//--------------------------------------------------------------
	void push( const vertex_array_holder& hold)
	{
		int b_count = hold.get_count();
		
		if ( glmode == hold.glmode)
		{
			if ( count+b_count < MAX_VERT - 4)
			{
				int& a = count;
				for (int b=0; b < b_count; a++,b++)
				{
					vert[a*2]   = hold.vert[b*2];
					vert[a*2+1] = hold.vert[b*2+1];
					
					color[a*4]  = hold.color[b*4];
					color[a*4+1]= hold.color[b*4+1];
					color[a*4+2]= hold.color[b*4+2];
					color[a*4+3]= hold.color[b*4+3];
				}
			}
			else
			{
				//DEBUG( "vertex_array_holder:push: buffer not large enough to push another vah.\n");
			}
		}
		else if ( glmode == GL_TRIANGLES &&
			hold.glmode == GL_TRIANGLE_STRIP)
		{
			if ( count+b_count*3 < MAX_VERT - 4)
			{
				int& a = count;
				for (int b=2; b < b_count; b++)
				{
					for ( int k=0; k<3; k++,a++)
					{
						int B = b-2 + k;
						
						vert[a*2]   = hold.vert[B*2];
						vert[a*2+1] = hold.vert[B*2+1];
						
						color[a*4]  = hold.color[B*4];
						color[a*4+1]= hold.color[B*4+1];
						color[a*4+2]= hold.color[B*4+2];
						color[a*4+3]= hold.color[B*4+3];
					}
				}
			}
			else
			{
				//DEBUG( "vertex_array_holder:push: buffer not large enough to push another vah.\n");
			}
		}
		else
		{
			//DEBUG( "vertex_array_holder:push: unknown type\n");
		}
	}
	//--------------------------------------------------------------
	ofVec2f get(int i)
	{
		ofVec2f P;
		P.x = vert[i*2];
		P.y = vert[i*2+1];
		return P;
	}
    //--------------------------------------------------------------
	ofFloatColor get_color(int b)
	{
		ofFloatColor C;
		C.r = color[b*4];
		C.g = color[b*4+1];
		C.b = color[b*4+2];
		C.a = color[b*4+3];
		return C;
	}
    //--------------------------------------------------------------
	ofVec2f get_relative_end(int di=-1)
	{	//di=-1 is the last one
		int i = count+di;
		if ( i<0) i=0;
		if ( i>=MAX_VERT) i=MAX_VERT-1;
		return get(i);
	}
    //--------------------------------------------------------------
	void repeat_last_push()
	{
		ofVec2f P;
		ofFloatColor cc;
		
		int i = count-1;
		
		P.x = vert[i*2];
		P.y = vert[i*2+1];
		
		cc.r = color[i*4];
		cc.g = color[i*4+1];
		cc.b = color[i*4+2];
		cc.a = color[i*4+3];
		
		push(P,cc);
	}
    //--------------------------------------------------------------
	void jump() //to make a jump in triangle strip by degenerated triangles
	{
		if ( glmode == GL_TRIANGLE_STRIP)
		{
			repeat_last_push();
			jumping=true;
		}
	}
	//--------------------------------------------------------------
	void draw() //the only place to call gl functions
	{
		if ( count > 0) //save some effort
		{
			glVertexPointer(2, GL_FLOAT, 0, vert);
			glColorPointer (4, GL_FLOAT, 0, color);
			glDrawArrays (glmode, 0, count);
		}
	}
};

#endif
