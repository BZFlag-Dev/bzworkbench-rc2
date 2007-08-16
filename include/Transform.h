#ifndef TRANSFORM_H_
#define TRANSFORM_H_

#include <string>
#include <vector>

#include "DataEntry.h"
#include "model/BZWParser.h"

#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>

using namespace std;

// a metadata entry that is used to describe a transformation such as spin, shear, shift, and scale
class BZTransform : public DataEntry, public osg::MatrixTransform {
	
	public:
	
		BZTransform() : DataEntry("", ""), osg::MatrixTransform() {
			name = string("");
			data = vector<float>();
		}
		
		BZTransform(string& data) : DataEntry("", ""), osg::MatrixTransform() {
			name = string("");
			this->data = vector<float>();
			this->update(data);
		}
		
		BZTransform( string name, vector<float> data ) : DataEntry("", ""), osg::MatrixTransform() {
			this->name = name;
			this->data = data;
			refreshMatrix();
		} 
		
		BZTransform( string name, float n1, float n2, float n3, float n4 ) : DataEntry("", ""), osg::MatrixTransform() {
			this->name = name;
			this->data = vector<float>();
			data.push_back(n1);
			data.push_back(n2);
			data.push_back(n3);
			data.push_back(n4);
			refreshMatrix();
		}
		
		// getter
		string get(void) {
			return this->toString();	
		}
		
		// setter
		int update(string& newData) {
			// expect just one line
			this->name = BZWParser::key(newData.c_str());
			
			// break up the line
			vector<string> elements = BZWParser::getLineElements(newData.c_str());
			
			// erase the current data
			data.clear();
			
			// get the numbers (don't start with elements.begin(), because this is what name is)
			for(vector<string>::iterator i = elements.begin() + 1; i != elements.end(); i++) {
				data.push_back( atof( i->c_str() ) );
			}
			
			this->setHeader(name.c_str());
			
			// recompute this matrix
			this->refreshMatrix();
			
			return 1;
		}
		
		// toString
		string toString(void) {
			string ret = string(name);
			for(vector<float>::iterator i = data.begin(); i != data.end(); i++) {
				ret += " " + string(ftoa(*i));
			}
			
			return ret + "\n";
		}
		
		// data getters
		string getName() { return name; }
		vector<float> getData() { return data; }
		
		// data setters
		void setName(string& s) { this->name = s; this->refreshMatrix(); }
		void setData(const vector<float>& data) { this->data = data; this->refreshMatrix(); }
		void setData( const osg::Vec3d& data ) {
			this->data.clear();
			this->data.push_back(data.x());
			this->data.push_back(data.y());
			this->data.push_back(data.z());
			this->refreshMatrix();
		}
		
		void setData( const osg::Vec4d& data ) {
			this->data.clear();
			this->data.push_back(data.x());
			this->data.push_back(data.y());
			this->data.push_back(data.z());
			this->data.push_back(data.w());
			this->refreshMatrix();
		}
		
		// make this public
		BZTransform operator =( const BZTransform& obj ) { 
			BZTransform newObj = BZTransform();
			memcpy(&newObj, &obj, sizeof(BZTransform));
			return newObj;
		}
		
	private:
	
		// make this into a shift matrix
		void makeShift() {
			
			osg::Matrixd theMatrix;
			theMatrix.makeTranslate( osg::Vec3( data[0], data[1], data[2] ) );
			
			this->setMatrix( theMatrix );
		}
		
		// make this into a scale matrix
		void makeScale() {
			osg::Matrixd theMatrix;
			theMatrix.makeScale( data[0], data[1], data[2] );
			this->setMatrix( theMatrix );
		}
		
		// make this into a spin matrix
		void makeSpin() {
			
			// use a quaternion to compute the "spin" matrix
			osg::Quat quat = osg::Quat( osg::DegreesToRadians(data[0]), osg::Vec3( data[1], data[2], data[3] ) );
			
			osg::Matrix matrix;
			matrix.makeRotate( quat );
			
			this->setMatrix( matrix );
			/*
			osg::Vec3 vec = osg::Vec3( data[1], data[2], data[3] );
			float len2 = vec.length2();
			float scale = 1.0f / sqrt(len2);
			
			osg::Vec3 n = osg::Vec3( vec.x() * scale, vec.y() * scale, vec.z() * scale );
			
			float cos_val = cos( osg::DegreesToRadians( data[0] ) );
			float sin_val = sin( osg::DegreesToRadians( data[0] ) );
			float icos_val = 1.0f - cos_val;
			
			// taken from BZFlag's MeshTransform.cxx file
			float t[4][4];
			t[3][3] = 1.0f;
			t[0][3] = t[1][3] = t[2][3] = 0.0f;
			t[3][0] = t[3][1] = t[3][2] = 0.0f;
			t[0][0] = (n.x() * n.x() * icos_val) + cos_val;
			t[0][1] = (n.x() * n.y() * icos_val) - (n.z() * sin_val);
			t[0][2] = (n.x() * n.z() * icos_val) + (n.y() * sin_val);
			t[1][0] = (n.y() * n.x() * icos_val) + (n.z() * sin_val);
			t[1][1] = (n.y() * n.y() * icos_val) + cos_val;
			t[1][2] = (n.y() * n.z() * icos_val) - (n.x() * sin_val);
			t[2][0] = (n.z() * n.x() * icos_val) - (n.y() * sin_val);
			t[2][1] = (n.z() * n.y() * icos_val) + (n.x() * sin_val);
			t[2][2] = (n.z() * n.z() * icos_val) + cos_val;
			
			float matVals[] = {
				t[0][0],	t[1][0],	t[2][0],	t[3][0],
				t[0][1],	t[1][1],	t[2][1],	t[3][1],
				t[0][2],	t[1][2],	t[2][2],	t[3][2],
				t[0][3],	t[1][3],	t[2][3],	t[3][3]
			};
			
			osg::Matrixd matrix = osg::Matrixd( matVals );
			this->setMatrix( matrix );*/
		}
		
		// make this into a shear matrix
		void makeShear() {
			double matvals[] = {
				1.0,		0.0,		data[2],	0.0,
				0.0,		1.0,		0.0,		0.0,
				data[0],	data[1],	1.0,		0.0,
				0.0,		0.0,		0.0,		1.0
			};
			
			osg::Matrixd matrix = osg::Matrixd( matvals );
			this->setMatrix( matrix );
		}
		
		
		string name;
		vector<float> data;
		vector<string> transformationFormats;
		
		void refreshMatrix() {
			if( name == "shift" )
				this->makeShift();
			else if(name == "spin" )
				this->makeSpin();
			else if(name == "shear" )
				this->makeShear();
			else if(name == "scale" )
				this->makeScale();
		}
};

#endif /*TRANSFORM_H_*/
