#version 410
#include <../image/header.glsl>
//self  = https://www.shadertoy.com/view/MdjcD3
//parent= https://www.shadertoy.com/view/XlV3Dy

/*
mouse does stuff.
cleaned and starting to undertand how to use:
substraction of distances is in line 235 ish
, mixing 2 shapes by sin(time)*.5+.5
*/

/*
arithmetic set for automatic differentiation, 
derivative arithmetic [DA] for
https://en.wikipedia.org/wiki/Smoothness
of shapes that (otherwise) lack C1-continuity.
based on    : https://www.shadertoy.com/view/4dVGzw
inspired by : https://www.shadertoy.com/view/Mdl3Ws 
explained at: http://blog.demofox.org/2014/12/30/dual-numbers-automatic-differentiation/
wiki        : https://en.m.wikipedia.org/wiki/Automatic_differentiation
summary:    : https://www.reddit.com/r/math/comments/68st6p/the_wikipedia_article_on_automatic/
*/

#define dimensions 3

#define IterRm 64
//max raymarch iterations|steps

#define IterSh 24
//softshadow iterations

#define StepFactor .99
//stepfactor range [.1 to 2.], where larger values tend to overstep.
//if(lipschitzConstant>1., distance is overestimated.
//... lazy fix for that is scaling stepDistance*=StepFactor
//I noticed better quality with a smaller stepfactor AND epsilon=0.
//meaning, a surface will NEVER really be reached.
//maybe todo: eps=log(eps*b)*a

#define Goursat
//toggle between 2 distance fields|surfaces:
//Goursat-surface (non-C1 continuous)
//or
//

//Using f'(x)= f(d)/dx notation for derivates:
//https://en.wikipedia.org/wiki/Derivative#Notation

//this defines common operations but on a (pseudo) dual number (i,j), defined as:
//[j] stays "underived"
//[i] calculates the gradient as byproduct
//[i] becomes Operation(i,FirstDerivateOf(Operation(i,j))
//[i] is always precise and this way usually calculates faster than using central differences,
//mostly because a lot of operations have a lot of values cancel each other out, allowing "shortcuts".
//especially canceling out a lot of domainVectors at once, which is nice for vectorHardware.
//
//"pseudo" dual because [i] has 3 dimensions, and [j] has 1 dimension that modifies on all dimensions of [i], 
//because graphic cards are good with domain operators on vectors.
//we define your dual number (where one part calculates with derivative of the other part and itself)
//as type vec4(vec3,float)
//and the 3 dimensions|domains are resolved with [struct DAVec3{}] and [da_domain()]:
struct DAVec3{vec4 x;vec4 y;vec4 z;};
DAVec3 da_domain(vec3 p){return DAVec3(
 vec4(1.,0.,0.,p.x),
 vec4(0.,1.,0.,p.y),
 vec4(0.,0.,1.,p.z));}
//this lets you derive over 3 dimensions at once OR seperately, using very self-similar dual-function variants:
//#define dacon(a) vec4(0.,0.,0.,a)
#define i1 in float
#define i4 in vec4
#define i3 in vec3

//automatic differenciation of paniq 
//transformed to float array form, wher paniq .w == array index #0.
#define a1 float[1]
#define a2 float[2]
#define a3 float[3]
#define a4 float[4]
//explicit vector proessing for array operands, because opengl is not C#
//return array fith entry #0 removed from it.
a3 a2m(a4 a){return a3(a[1],a[2],a[3]);}
a2 a2m(a3 a){return a2(a[1],a[2]);}
a1 a2m(a2 a){return a1(a[1]);}
float a2v(a1 a){return a[0];}vec4 a2v(a4 a){return vec4(a[0],a[1],a[2],a[3]);}
vec2 a2v(a2 a){return vec2(a[0],a[1]);}vec3 a2v(a3 a){return vec3(a[0],a[1],a[2]);}
#define a2n(a) a2v(a2m(a))
a1 v2a(float a){return a1(a);}a4 v2a(vec4 a){return a4(a.x,a.y,a.z,a.w);}
a2 v2a(vec2 a){return a2(a.x,a.y);}a3 v2a(vec3 a){return a3(a.x,a.y,a.z);}
#define a2ie return v2a(-a2v(a));} 
a1 a2i(a1 a){a2ie a2 a2i(a2 a){a2ie a3 a2i(a3 a){a2ie a4 a2i(a4 a){a2ie
//I hope this tortures a parser as much as possible.
/* //shorthand idea is on ice:
#define adda(z) z add(z a,a1 b){return v2a(a2v(a)+b[0]);}
adda(a1)adda(a2)adda(a3)adda(a4)
#define mula(z) z mul(z a,a1 b){return v2a(a2v(a)*b[0]);}
mula(a1)mula(a2)mula(a3)mula(a4)
*/
//this is NOT safeinv(), because safeinv is a recoprocal
//and i may have confused this a bit too often
float dac1(float z){return z;}
vec2 dac2(float z){return vec2(z,0.);}
vec3 dac3(float z){return vec3(z,0.,0.);}
vec4 dac4(float z){return vec4(z,vec3(0));}
//paniqs da uas .x as 0th derivative and all else as 1st derivative.
//but keep #0 as 0th derivative and all else as 1st derivative.
//for my array version that hopefully is more dynamic.
#define dasi2 sin(a[0]),a2n(a)*cos(a[0])));}
#define dasin(w) w da_sin(w a){return v2a
dasin(a4)(vec4(dasi2 dasin(a3)(vec3(dasi2 dasin(a2)(vec2(dasi2
#define daco2 cos(a[0]),-a2n(a)*sin(a[0])));}
#define dacos(w) w da_cos(w a){return v2a
dacos(a4)(vec4(daco2 dacos(a3)(vec3(daco2 dacos(a2)(vec2(daco2
//as proof of concept, remember the derivates of sin|cos:
//sin => cos => -sin => -cos => sin =>...
//vec4 da_sin(i4 a){return vec4( a.xyz*cos(a.w),sin(a.w));}
//vec4 da_cos(i4 a){return vec4(-a.xyz*sin(a.w),cos(a.w));}

//a4 daconsss(a1 a){return dac4(a[0]);}
#define daexp(z) z daex2(z a){float w=exp(a[0]);return v2a
daexp(a2)(vec2(w,a2n(a)*w));}
daexp(a3)(vec3(w,a2n(a)*w));}
daexp(a4)(vec4(w,a2n(a)*w));}
//vec4 da_exp(i4 a){float w=exp(a.w);return vec4(a.xyz*w,w);}//exp(a)==pow(e,a)
#define dalog2 log(a[0]),a2n(a)/a[0]));}
#define dalog3(z) z dalog(z a){return v2a
dalog3(a2)(vec2(dalog2 dalog3(a3)(vec3(dalog2 dalog3(a4)(vec4(dalog2
//vec4 da_log(i4 a){return vec4(a.xyz/a.w,log(a.w));}

//if(a==0)return a, return 1/a;
float safeinv(float a){return mix(a,1./a,step(abs(a),0.));}
a1 safeinv(a1 a){return v2a(safeinv(a[0]));}
#define daab2 abs(a[0]),a2n(a)*sign(a[0])));}
#define daab3(z) z daabs(z a){return v2a
daab3(a2)(vec2(daab2 daab3(a3)(vec3(daab2 daab3(a4)(vec4(daab2
//vec4 da_abs (i4 a){return vec4(a.xyz*sign(a.w),abs(a.w));}                                    
#define dapo2 a[0]*a[0],2.*a[0]*a2n(a)));}
#define dapo3(z) z dapow2(z a){return v2a
dapo3(a2)(vec2(dapo2 dapo3(a3)(vec3(dapo2 dapo3(a4)(vec4(dapo2
//vec4 da_pow2(i4 a){return vec4(2.*a.w*a.xyz, a.w*a.w);}
#define dasq2 sqrt(a[0]),safeinv(a[0])*a2n(a)*.5));}
#define dasq3(z) z dasqrt(z a){return v2a(
dasq3(a2)vec2(dasq2 dasq3(a3)vec3(dasq2 dasq3(a4)vec4(dasq2
//vec4 da_sqrt(i4 a){float q=sqrt(a.w);return vec4(.5*a.xyz*safeinv(q),q);}
#define damiE ,step(a[0],b[0])));}
#define damis(z) z damin(z a,z b){return v2a(mix(a2v(a),a2v(b)damiE
damis(a2)damis(a3)damis(a4)
//vec4 da_min(i4 a,i4 b){return(a.w<=b.w)?a:b;}
#define damiL(z) z damin(z a,a1 b){return v2a(mix(a2v(a)
damiL(a2),dac2(b[0])damiE
damiL(a3),dac3(b[0])damiE
damiL(a4),dac4(b[0])damiE
//vec4 da_min(i4 a,i1 b){return(a.w<=b  )?a:da_const(b);}
#define damiR(z) z damin(a1 a,z b){return v2a(mix
damiR(a2)(dac2(a[0]),a2v(b)damiE
damiR(a3)(dac3(a[0]),a2v(b)damiE
damiR(a4)(dac4(a[0]),a2v(b)damiE
//vec4 da_min(i1 a,i4 b){return(a  < b.w)?da_const(a):b;}
#define damaE ,step(b[0],a[0])));}
#define damas(z) z damax(z a,z b){return v2a(mix(a2v(a),a2v(b)damaE
damas(a2)damas(a3)damas(a4)
//vec4 da_max(i4 a,i1 b){return(a.w>=b  )?a:da_const(b);}
#define damaL(z) z damax(z a,a1 b){return v2a(mix(a2v(a)
damaL(a2),dac2(b[0])damaE
damaL(a3),dac3(b[0])damaE
damaL(a4),dac4(b[0])damaE
//da_max(i1 a,i4 b){return(a  > b.w)?da_const(a):b;}
#define damaR(z) z damax(a1 a,z b){return v2a(mix
damaR(a2)(dac2(a[0]),a2v(b)damaE
damaR(a3)(dac3(a[0]),a2v(b)damaE
damaR(a4)(dac4(a[0]),a2v(b)damaE
//vec4 da_min(i1 a,i4 b){return(a  < b.w)?da_const(a):b;}
#define mab0 mod(a[0],b[0])
#define damo2 mab0,mod(a2n(a),1.)));}
#define damo3(z) z damod(z a,z b){return v2a
damo3(a2)(vec2(damo2 damo3(a3)(vec3(damo2 damo3(a4)(vec4(damo2
//vec4 da_mod(i4 a,i4 b){return vec4(mod(a.xyz,1.),mod(a.w,b.w));}
#define damo1(z) z damod(z a,a1 b){return v2a
damo1(a2)(vec2(damo2 damo1(a3)(vec3(damo2 damo1(a4)(vec4(damo2
//vec4 da_mod(i4 a,i1 b){return vec4(mod(a.xyz,1.),mod(a.w,b  ));}
#define dam22 mod(a[0],b[0]),vec3(0)));}
#define damo0(z,w) z damod(a1 a,z b){return v2a(w(mab0,
damo0(a2,vec2)0));}damo0(a3,vec3)vec2(0)));}damo0(a4,vec4)vec3(0)));}
//vec4 da_mod(i1 a,i4 b){return vec4(0,0,0        ,mod(a  ,b.w));}//here "a.xyz"==vec3(0)
#define dase return v2a(a2v(a)-a2v(b));}
#define dasuM(z) z dasub(z a,z b)
dasuM(a2){dase dasuM(a3){dase dasuM(a4){dase
//vec4 da_sub(i4 a,i4 b){return a-b;}
#define dasu2 a[0]-b[0],a2n(a)));}
#define dasu3 a[0]-b[0],-a2n(b)));}
#define dasuR(z) z dasub(z a,a1 b){return v2a
#define dasuL(z) z dasub(a1 a,z b){return v2a
dasuR(a2)(vec2(dasu2 dasuR(a3)(vec3(dasu2 dasuR(a4)(vec4(dasu2
//vec4 da_sub(i4 a,i1 b){return vec4( a.xyz,a.w-b  );}
dasuL(a2)(vec2(dasu3 dasuL(a3)(vec3(dasu3 dasuL(a4)(vec4(dasu3
//vec4 da_sub(i1 a,i4 b){return vec4(-b.xyz,a  -b.w);}
#define daade return v2a(a2v(a)*a2v(b));}
#define daads(z) z daadd(z a,z b)
daads(a2){daade daads(a3){daade daads(a4){daade
//vec4 da_add(i4 a,i4 b){return a+b;}
#define daadE return dasub(a,a2i(b));}
#define daadL(z) z daadd(a1 a,z b){daadE
#define daadR(z) z daadd(z a,a1 b){daadE
daadL(a2)daadL(a3)daadL(a4)daadR(a2)daadR(a3)daadR(a4)
//vec4 da_add(i4 a,i1 b){return da_sub(a,-b);}//addition as invrse substraction
//vec4 da_add(i1 a,i4 b){return da_sub(a,-b);}
#define amue a2n(a)*b[0]+a2n(b)*a[0])));}
#define amun(z) z damul(z a,z b){return v2a
amun(a2)(vec2(a[0]*b[0],(amue
amun(a3)(vec3(a[0]*b[0],vec2(amue
amun(a4)(vec4(a[0]*b[0],vec3(amue
//vec4 da_mul(i4 a,i4 b){return vec4(a.xyz*b.w+a.w*b.xyz, a.w*b.w);}
#define amuf return v2a(a2v(a)*a2v(b));}
#define amuR(z) z damul(z a,a1 b){amuf
#define amuL(z) z damul(a1 a,z b){amuf
amuR(a2)amuR(a3)amuR(a4)amuL(a2)amuL(a3)amuL(a4)
//vec4 da_mul(i4 a,i1 b){return a*b;}
//vec4 da_mul(i1 a,i4 b){return a*b;}
#define adie a2n(a)*b[0]-a2n(b)*a[0])/(b[0]*b[0])));}
#define adin(z) z dadiv(z a,z b){return v2a
adin(a2)(vec2(a[0]/b[0],(adie
adin(a3)(vec3(a[0]/b[0],vec2(adie
adin(a4)(vec4(a[0]/b[0],vec3(adie
//vec4 da_div(i4 a,i4 b){return vec4((a.xyz*b.w-a.w*b.xyz)/(b.w*b.w), a.w/b.w);}
#define adiE return v2a(a2v(a)/a2v(b));}
#define adiR(z) z adiv(z a,a1 b)
adiR(a2){adiE adiR(a3){adiE adiR(a4){adiE
//vec4 da_div(i4 a,i1 b){return a/b;}
#define adil a[0]/b[0],-a[0]*a2n(b)/(b[0]*b[0])));}
#define adiL(z) z adiv(a1 a,z b){return v2a
adiL(a2)(vec2(adil adiL(a3)(vec3(adil adiL(a4)(vec4(adil
//vec4 da_div(i1 a,i4 b){return vec4((-a*b.xyz)/(b.w*b.w), a/b.w);} 
#define alw(w) a2n(w)*w[0]
#define al21(w) w dalen(w x){float q=abs(x[0]);return v2a(
#define aL21 safeinv(q)*alw(x),q));}
al21(a2)vec2(aL21 al21(a3)vec3(aL21 al21(a4)vec4(aL21
#define al22(w) w dalen(w x,w y){float q=length(vec2(x[0],y[0]));return v2a(
#define aL22 safeinv(q)*(alw(x)+alw(y)),q));}
al22(a2)vec2(aL22 al22(a3)vec3(aL22 al22(a4)vec4(aL22
vec4 da_length(i4 x,i4 y){float q=length(vec2(x.w,y.w));
 return vec4((x.xyz*x.w+y.xyz*y.w)*safeinv(q),q);}
#define al23(w) w dalen(w x,w y,w z){float q=length(vec3(x[0],y[0],z[0]));return v2a(
#define aL23 safeinv(q)*(alw(x)+alw(y)+alw(z)),q));}
al23(a2)vec2(aL23 al23(a3)vec3(aL23 al23(a4)vec4(aL23
vec4 da_length(i4 x,i4 y,i4 z){float q=length(vec3(x.w,y.w,z.w));
 return vec4((x.xyz*x.w+y.xyz*y.w+z.xyz*z.w)*safeinv(q),q);}

//back ocompatibility: (swivling is utterly wrong)
#define da_pow2(a) a2v(dapow2(v2a(a)))
#define da_add(a,b) a2v(daadd(v2a(a),v2a(b)))
#define da_sub(a,b) a2v(dasub(v2a(a),v2a(b)))
#define da_mul(a,b) a2v(damul(v2a(a),v2a(b)))
#define da_div(a,b) a2v(damul(v2a(a),v2a(b)))
#define da_min(a,b) a2v(damin(v2a(a),v2a(b)))
#define da_max(a,b) a2v(damax(v2a(a),v2a(b)))
#define da_abs(a) a2v(daabs(v2a(a)))
                                                 
#if (dimensions==3)
             
//using the above, we define distance functions that automatically calculate their derivatives
//this makes surfaces that are C1-discontinuous, like staircases, 
//at least have their (continuous) first rerivate calculated, which is useful for raymarching (?)
vec4 sdSphere(DAVec3 p){
 p.x*=.8;p.x-=.5*sin(1.61*iTime);
 p.y*=.8;
 p.z*=.8;p.z-=.5*cos(1.61*iTime);
 vec4 q=da_length(p.x,p.y,p.z);
 q=da_sub(q,1.);
 return q;}


// s: width, height, depth, thickness
// r: .xy corner radius, .z corner radius
//return distance to very parametric surface that contains many useful sub-shapes.
vec4 sdSuperprim(DAVec3 p, i4 s,vec2 r) {
    vec4 dx = da_sub(da_abs(p.x),s.x);
    vec4 dy = da_sub(da_abs(p.y),s.y);
    vec4 dz = da_sub(da_abs(p.z),s.z);
    vec4 q = 
       	da_sub(
            da_abs(
                da_add(
                    da_add(
                        da_length(
                            da_max(da_add(dx, r.x),0.),
                            da_max(da_add(dy, r.x),0.)),
                  		da_min(-r.x,da_max(dx,dy))),
                    s.w)), 
                s.w);
    return da_add(
                da_length(
                    da_max(da_add(q, r.y),0.0),
                    da_max(da_add(dz, r.y),0.0)),
                da_min(-r.y,da_max(q,dz)));
}

//Goursat Surface
vec4 sdGoursat(DAVec3 p) {
 p.x = da_mul(p.x, 2.0);
 p.y = da_mul(p.y, 2.0);
 p.z = da_mul(p.z, 2.0);
 vec4 d =da_add(
  20.*iMouse.x/iResolution.x,
  da_sub(
   da_add(
    da_pow2(
     da_pow2(p.x))
     ,da_add(
      da_pow2(
       da_pow2(p.y))
       ,da_pow2(
         da_pow2(p.z)))),
                da_add(
                    da_mul(5.0,da_pow2(p.x)),
                    da_add(
                        da_mul(5.0,da_pow2(p.y)),
                        da_mul(5.0,da_pow2(p.z))))));
    return da_div(d, max(11.8,length(d.xyz)));
}
#endif /*dimensions=3*/
             
// example parameters
#define SHAPE_COUNT 10.0
void getfactor (int i, out vec4 s,out vec2 r) {
    //i = 8;
    if (i == 0) { // cube
        s = vec4(1.0);
        r = vec2(0.0);
    } else if (i == 1) { // corridor
        s = vec4(vec3(1.0),0.25);
        r = vec2(0.0);
    } else if (i == 2) { // pipe
        s = vec4(vec3(1.0),0.25);
        r = vec2(1.0,0.0);
    } else if (i == 3) { // cylinder
        s = vec4(1.0);
        r = vec2(1.0,0.0);
	} else if (i == 4) { // pill
        s = vec4(1.0,1.0,2.0,1.0);
        r = vec2(1.0);
    } else if (i == 5) { // sphere
        s = vec4(1.0);
        r = vec2(1.0);
    } else if (i == 6) { // pellet
        s = vec4(1.0,1.0,0.25,1.0);
        r = vec2(1.0,0.25);
    } else if (i == 7) { // torus
        s = vec4(1.0,1.0,0.25,0.25);
        r = vec2(1.0,0.25);
    } else if (i == 8) { // sausage mouth
        s = vec4(2.0,0.5,0.25,0.25);
        r = vec2(0.5,0.25);
    } else if (i == 9) { // beveled O
        s = vec4(0.7,1.0,1.0,0.25);
        r = vec2(0.125);
	}
}


#if (dimensions==3)
//set camera position & target
void setCam(out vec3 o,out vec3 i,i1 t,i1 m){
 o=vec3(4.*sin(t),m*9.-2.,4.*cos(t));i=vec3(0);}


struct DAMValue{vec4 d;float m;};

DAMValue min2OLD(DAMValue a, DAMValue b){if(a.d.w>b.d.w)return b;return a;}
DAMValue planeOLD(DAVec3 p){return DAMValue(da_add(p.y,1.),1.);}
//DAMValue add_plane(DAVec3 p,DAMValue m){return min2(plane(p),m);}
DAMValue min2(DAMValue a, DAMValue b){if(a.d.x>b.d.x)return b;return a;}
DAMValue plane(DAVec3 p){return DAMValue(da_add(p.y,1.),1.);}
//DAMValue add_plane(DAVec3 p,DAMValue m){return min2(plane(p),m);}
                                                 
#define ss01(a) smoothstep(0.,1.,a) 
#define imsc(a) int(mod(a,SHAPE_COUNT))
//return distance to surface of DistanceField
DAMValue df(DAVec3 p){vec4 r;
#ifdef Goursat
 //r=sdGoursat(p);
 //r=sdSphere(p);
 //r=da_max(-sdTangle(p),sdSphere(p));
 //r=sdGoursat(p)*.7+sdSphere(p)*.3;
 r=mix(sdGoursat(p),sdSphere(p),sin(iTime*.61)*.5+.5);
  // r=r.wxyz;
#else
 float t=iTime,u=ss01(ss01(fract(t)));
 vec4 a,b;vec2 c,d;
 getfactor(imsc(t   ),a,c);
 getfactor(imsc(t+1.),b,d);
 DAVec3 q=DAVec3(p.z,p.y,p.x);
 if(iMouse.z>.5){
  vec2 m=iMouse.xy/iResolution.xy;
  r=sdSuperprim(q,vec4(vec3(1.0),mix(a.w,b.w,u)),m);
  //r=r.wxyz;
 }else
  r=sdSuperprim(q,mix(a,b,u),mix(c,d,u));
  //r=r.wxyz;
#endif
 return min2(plane(p),DAMValue(r,0.));}

//return distance & materialID
vec2 dm(i3 p){DAMValue d=df(da_domain(p));
 return vec2(d.d.w,d.m);}
//return material at p with normal n
vec4 material(i3 p,i3 n ){vec4 d=df(da_domain(vec3(p.x,0.,p.z))).d;
 return mix(vec4(n*.5+.5,.1)
           ,vec4(d.xyz*.5 +.5,.0)*abs(mod(d.w,.1)/.1-.5)
           ,clamp(dm(p).y,0.,1.));}

//return soft shadow
float shadow(i3 o,i3 i){
 const float a=32.;//shadow hardnes
 float r=1.,h =1.,t=.0005;//t=(self)intersection avoidance distance
 for(int j=0;j<IterSh;j++){
  h=dm(o+i*t).x;
  r=min(r,h*a/t);
  t+=clamp(h,.02,2.);}//limit max and min stepping distances
 return clamp(r,0.,1.);}

//return p, modified by light , shadow, envMap...
vec3 light(i3 p,i3 n,i3 i,i1 d,i4 m){
 vec3 l=normalize(vec3(1.,.7,.9));//light direction
 float o=max(0.,dot(n,l)),
 c=max(0.,dot(n,normalize(l-i))),
 s;if(c>.01)s=shadow(p+.01*n,l); 
 const vec3 LigCol=vec3(1.);//light color
 vec3 r=m.rgb*(LigCol+o*vec3(.8,.7,.6)*s*3.+vec3(.2,.3,.3));  
 r+=s*c*pow(o,40.);//specular
 //r+=m.w*texture(iChannel0, reflect(i,n)).rgb;//envmap
 r*=exp(-.01*d*d);//fog  
 return r;}

//return distance along ray (raymarch till surface)
float rm(i3 o,i3 i){
 const float m=20.;//distance.Max
 float e=.0,h=e,t=0.;//intersection.precission
 for(int j=0;j<IterRm;j++){
  if(h<e)break;if(t>m)return -1.;
  h=dm(o+i*t).x*StepFactor;t+=h;}return t;}

//return surface normal
vec3 getN(in vec3 p){DAMValue d=df(da_domain(p));
 return d.d.xyz;}

//return camera matrix
mat3 calcLookAtMatrix(in vec3 o,in vec3 i,in float r ){
 vec3 w=normalize(i-o);vec3 u=normalize(cross(w,vec3(sin(r),cos(r),0.)) );
 return mat3(u,normalize(cross(u,w)),w);}

vec3 demo3d(vec2 p,vec2 m){
 vec3 o,i;
 setCam(o,i,iTime,m.y+.2);//camMovement
 const float roll=0.,lens=2.;//camRoll,CamLensLength
 vec3 d=normalize(calcLookAtMatrix(o,i,roll)*vec3(p.xy,lens));//ray.direction
 vec3 c=vec3(0);
 float t=rm(o,d);//Distance.raymarched
 if(t>-.5){
  vec3 p=o+t*d,n=getN(p); //position & normal
  vec4 m=material(p,n);   //material
  c=light(p,n,d,t,m);}    //light
 return c;}
#else
//draw linear function and its first derivative.
vec3 demo2d(vec2 p,vec2 m){
 p*=5.;
 vec2 f=fract(abs(p)-.02);
 f*=(f*f)*(f*f)*(f*f);
 float grid=f.x+f.y;
 return vec3(grid);}
#endif /*dimensions==3*/


//return color corrected r
vec3 ff_filmic_gamma3(vec3 r){vec3 x=max(vec3(0),r-.004);
 return (x*(x*6.2+.5))/(x*(x*6.2+1.7)+.06);}

void mainImage( out vec4 r,in vec2 fragCoord ){
 vec2 p=(-iResolution.xy+2.*fragCoord.xy)/iResolution.y,
 m=iMouse.xy/iResolution.xy;  //mouse
 //m=vec2(.01,.2);//a debug overwrite
 #if (dimensions==3)
 vec3 c=demo3d(p,m);
 #else 
 vec3 c=demo2d(p,m);
 #endif /*dimensions==3*/
 c=ff_filmic_gamma3(c*.6);//gamma
 //c=pow( clamp(col,0.0,1.0), vec3(0.4545) );
 r=vec4(c,1.);}
#include <../image/footer.glsl>
