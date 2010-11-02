#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <math.h>

class Vector3D
{
	public:
                Vector3D()
                {
                    x = 0; y = 0; z = 0;
                }
		Vector3D(float x, float y, float z){
			this->x = x;
			this->y = y;
			this->z = z;
					
			float mag = sqrt(x*x+y*y+z*z);
			nx = x/mag;
			ny = y/mag;
			nz = z/mag;
                }
		
		float dot(Vector3D a){
			return a.nx * nx + a.ny * ny + a.nz * nz;
                }
		
		Vector3D cross(Vector3D a){
			return Vector3D(y*a.z - z*a.y, x*a.z - z*a.x, x*a.y - y*a.x);
                }
			
		float getX(){
			return x;
                }
				
		float getY(){
			return y;
                }
		
		float getZ(){
			return z;
                }
		
		float getMagn()	{
			return sqrt(x*x+y*y+z*z);
                }
		
		Vector3D operator- (Vector3D param) {
			Vector3D temp( x - param.getX(), y - param.getY(), z - param.getZ() );
			return (temp);
                }
				
	private:
		float x,y,z;
		float nx,ny,nz;
};

#endif // VECTOR3D_H
