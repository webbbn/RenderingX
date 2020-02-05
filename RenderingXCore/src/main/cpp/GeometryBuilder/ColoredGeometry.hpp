//
// Created by Constantin on 2/17/2019.
//

#ifndef FPV_VR_GLPROGRAMS_HELPER_GEOMETRYHELPER2_H
#define FPV_VR_GLPROGRAMS_HELPER_GEOMETRYHELPER2_H


#include "GLProgramVC.h"
#include "Color/Color.hpp"
#include <array>

class ColoredGeometry {
public:
    static const void makeColoredLine(GLProgramVC::Vertex array[],const int arrayOffset,const glm::vec3& point1,const glm::vec3& point2,
                                      const TrueColor color1,const TrueColor color2){
        auto& p0=array[arrayOffset];
        auto& p1=array[arrayOffset+1];
        p0.x=point1[0];
        p0.y=point1[1];
        p0.z=point1[2];
        p0.colorRGBA=color1;
        p1.x=point2[0];
        p1.y=point2[1];
        p1.z=point2[2];
        p1.colorRGBA=color2;
    };
    static const void makeColoredTriangle(GLProgramVC::Vertex array[],const int arrayOffset,
                                              const glm::vec3& point1,const glm::vec3& point2,const glm::vec3& point3,
                                          const TrueColor color1,const TrueColor color2,const TrueColor color3) {
        auto& p0=array[arrayOffset];
        auto& p1=array[arrayOffset+1];
        auto& p2=array[arrayOffset+2];
        p0.x = point1[0];
        p0.y = point1[1];
        p0.z = point1[2];
        p0.colorRGBA = color1;
        p1.x = point2[0];
        p1.y = point2[1];
        p1.z = point2[2];
        p1.colorRGBA = color2;
        p2.x = point3[0];
        p2.y = point3[1];
        p2.z = point3[2];
        p2.colorRGBA = color3;
    };
    static const void makeColoredRect(GLProgramVC::Vertex array[],const glm::vec3& point,const glm::vec3& width,const glm::vec3& height,const TrueColor color){
        glm::vec3 p1=glm::vec3(point[0],point[1],point[2]);
        glm::vec3 p2=glm::vec3(point[0]+height[0],point[1]+height[1],point[2]+height[2]);
        glm::vec3 p3=glm::vec3(point[0]+height[0]+width[0],point[1]+height[1]+width[1],point[2]+height[2]+width[2]);
        glm::vec3 p4=glm::vec3(point[0],point[1],point[2]);
        glm::vec3 p5=glm::vec3(point[0]+height[0]+width[0],point[1]+height[1]+width[1],point[2]+height[2]+width[2]);
        glm::vec3 p6=glm::vec3(point[0]+width[0],point[1]+width[1],point[2]+width[2]);
        makeColoredTriangle(array,0,p1,p2,p3,color,color,color);
        makeColoredTriangle(array,3,p4,p5,p6,color,color,color);
    };

    static const void makeColoredTriangle1(GLProgramVC::Vertex array[],const glm::vec3& point,const float width,const float height,const TrueColor color) {
        glm::vec3 p1=glm::vec3(point[0],point[1],point[2]);
        glm::vec3 p2=glm::vec3(point[0]+width,point[1],point[2]);
        glm::vec3 p3=glm::vec3(point[0]+width/2.0f,point[1]+height,point[2]);
        makeColoredTriangle(array,0,p1,p2,p3,color,color,color);
    }

private:
    //Creates the vertices forming a Grid with
    //N row(s)==tessellation and N column(s)==tessellation. Example:
    //Tessellation 0 : undefined
    //Tessellation 1 : [ ]
    //Tessellation 2 : [ | ]
    //Tessellation 3 : [ | | ]
    static std::vector<GLProgramVC::Vertex> createGridVertices(const unsigned int tessellation,const glm::vec3& point,float width,float height,const TrueColor color){
        const int tesselationX=tessellation;
        const int tesselationY=tessellation;
        float subW=width/(float)tesselationX;
        float subH=height/(float)tesselationY;
        std::vector<GLProgramVC::Vertex> vertices;
        for(int i=0;i<=tesselationX;i++){
            const float xPos=point.x+i*subW;
            for(int j=0;j<=tesselationY;j++){
                const float yPos=point.y+j*subH;
                vertices.push_back({xPos,yPos,point.z,color});
            }
        }
        return vertices;
    }
    //create indices that can be used to draw a grid generated by createGridVertices()
    //GL_TRIANGLES render a solid plane
    static std::vector<unsigned int> createIndicesPlane(const unsigned int tessellation){
        const int indicesX=tessellation;
        const int indicesY=tessellation;
        const int rowSize=tessellation+1;
        int count=0;
        std::vector<unsigned int> indices(6*tessellation*tessellation);
        for(int i=0;i<indicesX;i++){
            for(int j=0;j<indicesY;j++){
                indices.at(count++)=(GLushort)(i*rowSize+j);
                indices.at(count++)=(GLushort)((i+1)*rowSize+j);
                indices.at(count++)=(GLushort)(i*rowSize+j+1);
                indices.at(count++)=(GLushort)(i*rowSize+j+1);
                indices.at(count++)=(GLushort)((i+1)*rowSize+j);
                indices.at(count++)=(GLushort)((i+1)*rowSize+j+1);
            }
        }
        return indices;
    }
    //create indices that can be used to draw a grid generated by createGridVertices()
    //GL_LINES creating a wireframe grid
    static std::vector<unsigned int> createIndicesWireframe(const unsigned int tessellation){
        //Create the indices forming vertical lines
        const int tessellationPlus1=tessellation+1;
        std::vector<unsigned int> indices;
        for(int i=0;i<tessellationPlus1;i++){
            const auto begin=i*tessellationPlus1;
            for(int j=0;j<tessellation;j++){
                indices.push_back(begin+j);
                indices.push_back(begin+j+1);
            }
        }
        //Create the indices forming horizontal lines
        for(int i=0;i<tessellationPlus1;i++){
            const auto begin=i;
            for(int j=0;j<tessellation;j++){
                indices.push_back(begin+j*tessellationPlus1);
                indices.push_back(begin+(j+1)*tessellationPlus1);
            }
        }
        return indices;
    }

    //Takes vertices and indices and returns an array of vertices that can be drawn without indices
    static std::vector<GLProgramVC::Vertex> mergeIndicesIntoVertices(const std::vector<GLProgramVC::Vertex>& vertices,const std::vector<unsigned int>& indices){
        std::vector<GLProgramVC::Vertex> ret;
        ret.reserve(indices.size());
        for(unsigned int index:indices){
            if(index>=vertices.size()){
                LOGD("Error wanted %d",index);
            }
            ret.push_back(vertices.at(index));
        }
        return ret;
    }
    //create indices that can be used to draw a wireframe plane generated by createGridVertices()
public:
    static const std::vector<GLProgramVC::Vertex> makeTesselatedColoredRect(const unsigned int tessellation,const glm::vec3& point,float width,float height,const TrueColor color){
        const auto vertices=createGridVertices(tessellation,point,width,height,color);
        const auto indices=createIndicesPlane(tessellation);
        //we do not want indices, remove them
        return mergeIndicesIntoVertices(vertices,indices);
    }
    //Can be drawn using GL_LINES
    static const std::vector<GLProgramVC::Vertex> makeTesselatedColoredRectWireframe(const unsigned int tessellation,const glm::vec3& point,float width,float height,const TrueColor color){
        const auto vertices=createGridVertices(tessellation,point,width,height,color);
        const auto indices=createIndicesWireframe(tessellation);
        //we do not want indices, remove them
        return mergeIndicesIntoVertices(vertices,indices);
    }

    //Create a tesselated wireframe mesh that spawns exactly the viewport
    //e.q [-1,1] in x and y direction
    static const std::vector<GLProgramVC::Vertex> makeTesselatedColoredRectLinesNDC(
            const int tessellation, const TrueColor color){
        return makeTesselatedColoredRectWireframe(tessellation,{-1.0f,-1.0f,0},2.0f,2.0f,color);
    }

    static const void makeOutlineQuadWithLines(GLProgramVC::Vertex array[],const float mX,const float mY,const float mZ,const float quadWith,const float quadHeight,
                                               const TrueColor color){
        //left and right
        makeColoredLine(array, 0 * 2, glm::vec3(mX, mY, mZ), glm::vec3(mX, mY + quadHeight, mZ),color, color);
        makeColoredLine(array, 1 * 2, glm::vec3(mX + quadWith, mY, mZ),glm::vec3(mX + quadWith, mY + quadHeight, mZ), color, color);
        //top and bottom
        makeColoredLine(array, 2 * 2, glm::vec3(mX, mY + quadHeight, mZ),glm::vec3(mX + quadWith, mY + quadHeight, mZ), color, color);
        makeColoredLine(array, 3 * 2, glm::vec3(mX, mY, mZ), glm::vec3(mX + quadWith, mY, mZ),color, color);
    }
    static const void makeBackgroundRect(GLProgramVC::Vertex array[],const glm::vec3& point,const float width,const float height,const TrueColor color1,const TrueColor color2){
        //p4    p1
        //   p5
        //p3    p2
        glm::vec3 p1=glm::vec3(point.x+width,point.y+height,point.z);
        glm::vec3 p2=glm::vec3(point.x+width,point.y,point.z);
        glm::vec3 p3=glm::vec3(point.x,point.y,point.z);
        glm::vec3 p4=glm::vec3(point.x,point.y+height,point.z);
        glm::vec3 p5=glm::vec3(point.x+width/2.0f,point.y+height/2.0f,point.z);
        makeColoredTriangle(array,0*3,p5,p1,p4,color1,color2,color2);
        makeColoredTriangle(array,1*3,p5,p1,p2,color1,color2,color2);
        makeColoredTriangle(array,2*3,p5,p2,p3,color1,color2,color2);
        makeColoredTriangle(array,3*3,p5,p4,p3,color1,color2,color2);
    };
    static std::vector<GLProgramVC::Vertex> makeDebugCoordinateSystemLines(const int tessellation=1){
        std::vector<GLProgramVC::Vertex> ret;
        ret.reserve(((tessellation+1)*(tessellation+1)));
        const float size=10.0f;
        const float stepSize=size/tessellation;
        const auto color=Color::BLUE;
        for(int i=0;i<tessellation;i++){
            const float x=(-size/2.f)+i*stepSize;
            const float x2=(-size/2.f)+(i+1)*stepSize;
            GLProgramVC::Vertex line[2];
            ColoredGeometry::makeColoredLine(line,0,glm::vec3(x,0,0),glm::vec3(x2,0,0),color,color);
            ret.push_back(line[0]);
            ret.push_back(line[1]);
            ColoredGeometry::makeColoredLine(line,0,glm::vec3(0,x,0),glm::vec3(0,x2,0),color,color);
            ret.push_back(line[0]);
            ret.push_back(line[1]);
        }
        return ret;
    }
};


#endif //FPV_VR_GLPROGRAMS_HELPER_GEOMETRYHELPER2_H
