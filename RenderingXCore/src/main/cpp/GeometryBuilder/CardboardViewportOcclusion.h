//
// Created by Constantin on 1/14/2020.
//

#ifndef RENDERINGX_CARDBOARDVIEWPORTOCCLUSION_H
#define RENDERINGX_CARDBOARDVIEWPORTOCCLUSION_H

#import "../DistortionCorrection/VRHeadsetParams.h"


class CardboardViewportOcclusion{
public:
    //Creates a Mesh using Normalized device coordinates that occludes
    //everything except the part actually visible inside the headset
    //Vertex data can be rendered using GL_TRAINGLE_STRIP
    //TODO: Better documentation of the used algorithm
    static const std::vector<GLProgramVC::Vertex> makeMesh(const VRHeadsetParams& params,const int eye,const TrueColor color=Color::BLACK){

        //create the 4 meshes (left,right,top,bottom) that are
        //later merged into one big mesh
        std::array<std::vector<GLProgramVC::Vertex>,4> mesh;

        //Make two vertical (left and right)
        mesh.at(0)=makeSomething({-1,-1},2.0f,false,color);
        mesh.at(1)=makeSomething({1,-1},2.0f,false,color);
        //Make two horizontal (top and bottom)
        mesh.at(2)=makeSomething({-1,1},2.0f,true,color);
        mesh.at(3)=makeSomething({-1,-1},2.0f,true,color);

        //Distort every second vertex
        for(auto& tmp:mesh){
            for(int i=1;i<tmp.size();i+=2){
                GLProgramVC::Vertex& v=tmp.at(i);
                auto xy=params.UndistortedNDCForDistortedNDC({v.x,v.y},eye);
                v.x=xy[0];
                v.y=xy[1];
            }
        }

        //merge them together
        //using degenerate triangles in between
        std::vector<GLProgramVC::Vertex> ret;
        for(int i=0;i<4;i++){
            const auto& tmp=mesh.at(i);
            //add first vertex to degenerate, except first merge
            if(i>0){
                ret.push_back(tmp.at(0));
            }
            ret.insert(ret.end(),tmp.begin(),tmp.end());
            //add last vertex to degenerate except last merge
            if(i<3){
                ret.push_back(tmp.back());
            }
        }
        return ret;
    }

    // V1--V3--V5-- .... VN
    //  |  |   |          |
    // V2--V4--V6-- ...  VN
    static const std::vector<GLProgramVC::Vertex> makeSomething(const glm::vec2 start,const float size,const bool horizontal,const TrueColor color){
        //create a strip in the form of
        //1   | 0 1
        //0.5 | 2 3
        //0   | 4 5
        //-0.5| 6 7
        //-1  | 8 9
        //0.5 0.25 0.125 ...
        const float stepSize=size/8;
        std::vector<GLProgramVC::Vertex> ret;
        for(float i=0;i<=size;i+=stepSize){
            if(horizontal){
                GLProgramVC::Vertex v{
                        start.x+i,start.y,0,color
                };
                ret.push_back(v);
                ret.push_back(v);
            }else{
                GLProgramVC::Vertex v{
                        start.x,start.y+i,0,color
                };
                ret.push_back(v);
                ret.push_back(v);
            }
        }
        return ret;
    }
};


#endif //RENDERINGX_CARDBOARDVIEWPORTOCCLUSION_H