/*1:*/
#line 106 "./voc.w"

/*3:*/
#line 22 "./data.w"

#include <stdlib.h> 
#include <math.h> 
#include <string.h> 
#include <soundpipe.h> 
#include <sporth.h> 

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "voc.h"

#ifndef MIN
#define MIN(A,B) ((A) < (B) ? (A) : (B))
#endif

#ifndef MAX
#define MAX(A,B) ((A) >  (B) ? (A) : (B))
#endif

#define EPSILON 1.0e-38

/*4:*/
#line 50 "./data.w"

/*6:*/
#line 91 "./data.w"


typedef struct{
SPFLOAT freq;
SPFLOAT tenseness;
SPFLOAT Rd;
SPFLOAT waveform_length;
SPFLOAT time_in_waveform;

SPFLOAT alpha;
SPFLOAT E0;
SPFLOAT epsilon;
SPFLOAT shift;
SPFLOAT delta;
SPFLOAT Te;
SPFLOAT omega;

SPFLOAT T;
}glottis;

/*:6*/
#line 51 "./data.w"

/*7:*/
#line 112 "./data.w"

typedef struct{
int n;

SPFLOAT diameter[44];
SPFLOAT rest_diameter[44];
SPFLOAT target_diameter[44];
SPFLOAT new_diameter[44];
SPFLOAT R[44];
SPFLOAT L[44];
SPFLOAT reflection[45];
SPFLOAT new_reflection[45];
SPFLOAT junction_outL[45];
SPFLOAT junction_outR[45];
SPFLOAT A[44];
SPFLOAT max_amplitude[44];

int nose_length;


int nose_start;

int tip_start;
SPFLOAT noseL[28];
SPFLOAT noseR[28];
SPFLOAT nose_junc_outL[29];
SPFLOAT nose_junc_outR[29];
SPFLOAT nose_reflection[29];
SPFLOAT nose_diameter[28];
SPFLOAT noseA[28];
SPFLOAT nose_max_amp[28];

SPFLOAT reflection_left;
SPFLOAT reflection_right;
SPFLOAT reflection_nose;

SPFLOAT new_reflection_left;
SPFLOAT new_reflection_right;
SPFLOAT new_reflection_nose;

SPFLOAT velum_target;

SPFLOAT glottal_reflection;
SPFLOAT lip_reflection;
SPFLOAT last_obstruction;
SPFLOAT fade;
SPFLOAT movement_speed;
SPFLOAT lip_output;
SPFLOAT nose_output;
SPFLOAT block_time;
}tract;

#line 113 "./voc.w"

#line 1 "./top.w"
/*:7*/
#line 52 "./data.w"

/*5:*/
#line 59 "./data.w"


struct sp_voc{
glottis glot;
tract tr;
SPFLOAT buf[512];
int counter;
};

/*:5*/
#line 53 "./data.w"


/*:4*/
#line 45 "./data.w"


/*:3*/
#line 107 "./voc.w"

/*28:*/
#line 9 "./glottis.w"

/*31:*/
#line 78 "./glottis.w"

static void glottis_setup_waveform(glottis*glot,SPFLOAT lambda)
{
/*32:*/
#line 96 "./glottis.w"

SPFLOAT Rd;
SPFLOAT Ra;
SPFLOAT Rk;
SPFLOAT Rg;

SPFLOAT Ta;
SPFLOAT Tp;
SPFLOAT Te;

SPFLOAT epsilon;
SPFLOAT shift;
SPFLOAT delta;
SPFLOAT rhs_integral;

SPFLOAT lower_integral;
SPFLOAT upper_integral;

SPFLOAT omega;
SPFLOAT s;
SPFLOAT y;
SPFLOAT z;

SPFLOAT alpha;
SPFLOAT E0;

/*:32*/
#line 81 "./glottis.w"

/*33:*/
#line 136 "./glottis.w"

glot->Rd= 3*(1-glot->tenseness);
glot->waveform_length= 1.0/glot->freq;

Rd= glot->Rd;
if(Rd<0.5)Rd= 0.5;
if(Rd> 2.7)Rd= 2.7;

/*:33*/
#line 82 "./glottis.w"

/*34:*/
#line 158 "./glottis.w"

Ra= -0.01+0.048*Rd;
Rk= 0.224+0.118*Rd;
Rg= (Rk/4)*(0.5+1.2*Rk)/(0.11*Rd-Ra*(0.5+1.2*Rk));

/*:34*/
#line 83 "./glottis.w"

/*35:*/
#line 170 "./glottis.w"

Ta= Ra;
Tp= (SPFLOAT)1.0/(2*Rg);
Te= Tp+Tp*Rk;

/*:35*/
#line 84 "./glottis.w"


/*36:*/
#line 175 "./glottis.w"

epsilon= (SPFLOAT)1.0/Ta;
shift= exp(-epsilon*(1-Te));
delta= 1-shift;

/*:36*/
#line 86 "./glottis.w"


/*37:*/
#line 180 "./glottis.w"

rhs_integral= (SPFLOAT)(1.0/epsilon)*(shift-1)+(1-Te)*shift;
rhs_integral= rhs_integral/delta;
lower_integral= -(Te-Tp)/2+rhs_integral;
upper_integral= -lower_integral;

/*:37*/
#line 88 "./glottis.w"

/*38:*/
#line 191 "./glottis.w"

omega= M_PI/Tp;
s= sin(omega*Te);

y= -M_PI*s*upper_integral/(Tp*2);
z= log(y);
alpha= z/(Tp/2-Te);
E0= -1/(s*exp(alpha*Te));

/*:38*/
#line 89 "./glottis.w"


/*39:*/
#line 200 "./glottis.w"

glot->alpha= alpha;
glot->E0= E0;
glot->epsilon= epsilon;
glot->shift= shift;
glot->delta= delta;
glot->Te= Te;
glot->omega= omega;
#line 117 "./voc.w"

#line 1 "./tract.w"
/*:39*/
#line 91 "./glottis.w"

}

/*:31*/
#line 10 "./glottis.w"

/*29:*/
#line 16 "./glottis.w"

static void glottis_init(glottis*glot,SPFLOAT sr)
{
glot->freq= 140;
glot->tenseness= 0.6;
glot->T= 1.0/sr;
glot->time_in_waveform= 0;
glottis_setup_waveform(glot,0);
}

/*:29*/
#line 11 "./glottis.w"

/*30:*/
#line 31 "./glottis.w"

static SPFLOAT glottis_compute(sp_data*sp,glottis*glot,SPFLOAT lambda)
{
SPFLOAT out;
SPFLOAT aspiration;
SPFLOAT noise;
SPFLOAT t;
SPFLOAT intensity;

out= 0;
intensity= 1.0;
glot->time_in_waveform+= glot->T;

if(glot->time_in_waveform> glot->waveform_length){
glot->time_in_waveform-= glot->waveform_length;
glottis_setup_waveform(glot,lambda);

}

t= (glot->time_in_waveform/glot->waveform_length);

if(t> glot->Te){
out= (-exp(-glot->epsilon*(t-glot->Te))+glot->shift)/glot->delta;
}else{
out= glot->E0*exp(glot->alpha*t)*sin(glot->omega*t);
}




noise= 2.0*((SPFLOAT)sp_rand(sp)/SP_RANDMAX)-1;


aspiration= intensity*(1-sqrt(glot->tenseness))*0.3*noise;

aspiration*= 0.2;

out+= aspiration;

return out;
}

/*:30*/
#line 12 "./glottis.w"


/*:28*/
#line 108 "./voc.w"

/*40:*/
#line 11 "./tract.w"

/*52:*/
#line 222 "./tract.w"

static void tract_calculate_reflections(tract*tr)
{
int i;
SPFLOAT sum;

for(i= 0;i<tr->n;i++){
tr->A[i]= tr->diameter[i]*tr->diameter[i];

}

for(i= 1;i<tr->n;i++){
tr->reflection[i]= tr->new_reflection[i];
if(tr->A[i]==0){
tr->new_reflection[i]= 0.999;
}else{
tr->new_reflection[i]= 
(tr->A[i-1]-tr->A[i])/(tr->A[i-1]+tr->A[i]);
}
}

tr->reflection_left= tr->new_reflection_left;
tr->reflection_right= tr->new_reflection_right;
tr->reflection_nose= tr->new_reflection_nose;

sum= tr->A[tr->nose_start]+tr->A[tr->nose_start+1]+tr->noseA[0];
tr->new_reflection_left= (SPFLOAT)(2*tr->A[tr->nose_start]-sum)/sum;
tr->new_reflection_right= (SPFLOAT)(2*tr->A[tr->nose_start+1]-sum)/sum;
tr->new_reflection_nose= (SPFLOAT)(2*tr->noseA[0]-sum)/sum;
}

/*:52*/
#line 12 "./tract.w"

/*53:*/
#line 258 "./tract.w"

static void tract_calculate_nose_reflections(tract*tr)
{
int i;

for(i= 0;i<tr->nose_length;i++){
tr->noseA[i]= tr->nose_diameter[i]*tr->nose_diameter[i];
}

for(i= 1;i<tr->nose_length;i++){
tr->nose_reflection[i]= (tr->noseA[i-1]-tr->noseA[i])/
(tr->noseA[i-1]+tr->noseA[i]);
}
}

/*:53*/
#line 13 "./tract.w"

/*54:*/
#line 275 "./tract.w"


static SPFLOAT move_towards(SPFLOAT current,SPFLOAT target,
SPFLOAT amt_up,SPFLOAT amt_down)
{
SPFLOAT tmp;
if(current<target){
tmp= current+amt_up;
return MIN(tmp,target);
}else{
tmp= current-amt_down;
return MAX(tmp,target);
}
return 0.0;
}

static void tract_reshape(tract*tr)
{
SPFLOAT amount;
SPFLOAT slow_return;
SPFLOAT diameter;
SPFLOAT target_diameter;
int i;



amount= tr->block_time*tr->movement_speed;

for(i= 0;i<tr->n;i++){
slow_return= 0;
diameter= tr->diameter[i];
target_diameter= tr->target_diameter[i];



if(i<tr->nose_start)slow_return= 0.6;
else if(i>=tr->tip_start)slow_return= 1.0;
else{
slow_return= 
0.6+0.4*(i-tr->nose_start)/(tr->tip_start-tr->nose_start);
}

tr->diameter[i]= move_towards(diameter,target_diameter,
slow_return*amount,2*amount);

}

tr->nose_diameter[0]= move_towards(tr->nose_diameter[0],tr->velum_target,
amount*0.25,amount*0.1);
tr->noseA[0]= tr->nose_diameter[0]*tr->nose_diameter[0];
}
#line 119 "./voc.w"

#line 1 "./header.w"
/*:54*/
#line 14 "./tract.w"

/*41:*/
#line 21 "./tract.w"

static void tract_init(sp_data*sp,tract*tr)
{
int i;
SPFLOAT diameter,d;
/*42:*/
#line 38 "./tract.w"

tr->n= 44;
tr->nose_length= 28;
tr->nose_start= 17;

tr->reflection_left= 0.0;
tr->reflection_right= 0.0;
tr->reflection_nose= 0.0;
tr->new_reflection_left= 0.0;
tr->new_reflection_right= 0.0;
tr->new_reflection_nose= 0.0;
tr->velum_target= 0.01;
tr->glottal_reflection= 0.75;
tr->lip_reflection= -0.85;
tr->last_obstruction= -1;
tr->movement_speed= 15;
tr->lip_output= 0;
tr->nose_output= 0;
tr->tip_start= 32;

/*:42*/
#line 26 "./tract.w"

/*43:*/
#line 61 "./tract.w"

memset(tr->diameter,0,tr->n*sizeof(SPFLOAT));
memset(tr->rest_diameter,0,tr->n*sizeof(SPFLOAT));
memset(tr->target_diameter,0,tr->n*sizeof(SPFLOAT));
memset(tr->new_diameter,0,tr->n*sizeof(SPFLOAT));
memset(tr->L,0,tr->n*sizeof(SPFLOAT));
memset(tr->R,0,tr->n*sizeof(SPFLOAT));
memset(tr->reflection,0,(tr->n+1)*sizeof(SPFLOAT));
memset(tr->new_reflection,0,(tr->n+1)*sizeof(SPFLOAT));
memset(tr->junction_outL,0,(tr->n+1)*sizeof(SPFLOAT));
memset(tr->junction_outR,0,(tr->n+1)*sizeof(SPFLOAT));
memset(tr->A,0,tr->n*sizeof(SPFLOAT));
memset(tr->max_amplitude,0,tr->n*sizeof(SPFLOAT));
memset(tr->noseL,0,tr->nose_length*sizeof(SPFLOAT));
memset(tr->noseR,0,tr->nose_length*sizeof(SPFLOAT));
memset(tr->nose_junc_outL,0,(tr->nose_length+1)*sizeof(SPFLOAT));
memset(tr->nose_junc_outR,0,(tr->nose_length+1)*sizeof(SPFLOAT));
memset(tr->nose_diameter,0,tr->nose_length*sizeof(SPFLOAT));
memset(tr->noseA,0,tr->nose_length*sizeof(SPFLOAT));
memset(tr->nose_max_amp,0,tr->nose_length*sizeof(SPFLOAT));

/*:43*/
#line 27 "./tract.w"

/*44:*/
#line 90 "./tract.w"

for(i= 0;i<tr->n;i++){
diameter= 0;
if(i<7*(SPFLOAT)tr->n/44-0.5){
diameter= 0.6;
}else if(i<12*(SPFLOAT)tr->n/44){
diameter= 1.1;
}else{
diameter= 1.5;
}

tr->diameter[i]= 
tr->rest_diameter[i]= 
tr->target_diameter[i]= 
tr->new_diameter[i]= diameter;

}

/*:44*/
#line 28 "./tract.w"

/*45:*/
#line 116 "./tract.w"

for(i= 0;i<tr->nose_length;i++){
d= 2*((SPFLOAT)i/tr->nose_length);
if(d<1){
diameter= 0.4+1.6*d;
}else{
diameter= 0.5+1.5*(2-d);
}
diameter= MIN(diameter,1.9);
tr->nose_diameter[i]= diameter;
}

/*:45*/
#line 29 "./tract.w"


tract_calculate_reflections(tr);
tract_calculate_nose_reflections(tr);
tr->nose_diameter[0]= tr->velum_target;

tr->block_time= 512.0/(SPFLOAT)sp->sr;
}

/*:41*/
#line 15 "./tract.w"

/*46:*/
#line 138 "./tract.w"

static void tract_compute(sp_data*sp,tract*tr,
SPFLOAT in,
SPFLOAT lambda)
{
SPFLOAT r,w;
int i;


/*47:*/
#line 158 "./tract.w"

tr->junction_outR[0]= tr->L[0]*tr->glottal_reflection+in;
tr->junction_outL[tr->n]= tr->R[tr->n-1]*tr->lip_reflection;

for(i= 1;i<tr->n;i++){
r= tr->reflection[i]*(1-lambda)+tr->new_reflection[i]*lambda;
w= r*(tr->R[i-1]+tr->L[i]);
tr->junction_outR[i]= tr->R[i-1]-w;
tr->junction_outL[i]= tr->L[i]+w;
}

/*:47*/
#line 147 "./tract.w"

/*48:*/
#line 169 "./tract.w"

i= tr->nose_start;
r= tr->new_reflection_left*(1-lambda)+tr->reflection_left*lambda;
tr->junction_outL[i]= r*tr->R[i-1]+(1+r)*(tr->noseL[0]+tr->L[i]);
r= tr->new_reflection_right*(1-lambda)+tr->reflection_right*lambda;
tr->junction_outR[i]= r*tr->L[i]+(1+r)*(tr->R[i-1]+tr->noseL[0]);
r= tr->new_reflection_nose*(1-lambda)+tr->reflection_nose*lambda;
tr->nose_junc_outR[0]= r*tr->noseL[0]+(1+r)*(tr->L[i]+tr->R[i-1]);

/*:48*/
#line 148 "./tract.w"

/*49:*/
#line 178 "./tract.w"

for(i= 0;i<tr->n;i++){
tr->R[i]= tr->junction_outR[i]*0.999;
tr->L[i]= tr->junction_outL[i+1]*0.999;
}
tr->lip_output= tr->R[tr->n-1];

/*:49*/
#line 149 "./tract.w"

/*50:*/
#line 185 "./tract.w"

tr->nose_junc_outL[tr->nose_length]= 
tr->noseR[tr->nose_length-1]*tr->lip_reflection;

for(i= 1;i<tr->nose_length;i++){
w= tr->nose_reflection[i]*(tr->noseR[i-1]+tr->noseL[i]);
tr->nose_junc_outR[i]= tr->noseR[i-1]-w;
tr->nose_junc_outL[i]= tr->noseL[i]+w;
}

/*:50*/
#line 150 "./tract.w"

/*51:*/
#line 195 "./tract.w"

for(i= 0;i<tr->nose_length;i++){
tr->noseR[i]= tr->nose_junc_outR[i];
tr->noseL[i]= tr->nose_junc_outL[i+1];
}
tr->nose_output= tr->noseR[tr->nose_length-1];

/*:51*/
#line 151 "./tract.w"

}

/*:46*/
#line 16 "./tract.w"


/*:40*/
#line 109 "./voc.w"

/*8:*/
#line 10 "./top.w"

/*9:*/
#line 33 "./top.w"

int sp_voc_create(sp_voc**voc)
{
*voc= malloc(sizeof(sp_voc));
return SP_OK;
}

/*:9*/
#line 11 "./top.w"

/*10:*/
#line 43 "./top.w"

int sp_voc_destroy(sp_voc**voc)
{
free(*voc);
return SP_OK;
}

/*:10*/
#line 12 "./top.w"

/*11:*/
#line 53 "./top.w"

int sp_voc_init(sp_data*sp,sp_voc*voc)
{
glottis_init(&voc->glot,sp->sr);
tract_init(sp,&voc->tr);
voc->counter= 0;
return SP_OK;
}

/*:11*/
#line 13 "./top.w"

/*12:*/
#line 66 "./top.w"

int sp_voc_compute(sp_data*sp,sp_voc*voc,SPFLOAT*out)
{
SPFLOAT vocal_output,glot;
SPFLOAT lambda1,lambda2;
int i;



if(voc->counter==0){
tract_reshape(&voc->tr);
tract_calculate_reflections(&voc->tr);
for(i= 0;i<512;i++){
vocal_output= 0;
lambda1= (SPFLOAT)i/512;
lambda2= (SPFLOAT)(i+0.5)/512;
glot= glottis_compute(sp,&voc->glot,lambda1);

tract_compute(sp,&voc->tr,glot,lambda1);
vocal_output+= voc->tr.lip_output+voc->tr.nose_output;

tract_compute(sp,&voc->tr,glot,lambda2);
vocal_output+= voc->tr.lip_output+voc->tr.nose_output;
voc->buf[i]= vocal_output*0.125;
}
}


*out= voc->buf[voc->counter];
voc->counter= (voc->counter+1)%512;
return SP_OK;
}

/*:12*/
#line 14 "./top.w"

/*13:*/
#line 104 "./top.w"

int sp_voc_tract_compute(sp_data*sp,sp_voc*voc,SPFLOAT*in,SPFLOAT*out)
{
SPFLOAT vocal_output;
SPFLOAT lambda1,lambda2;

if(voc->counter==0){
tract_reshape(&voc->tr);
tract_calculate_reflections(&voc->tr);
}

vocal_output= 0;
lambda1= (SPFLOAT)voc->counter/512;
lambda2= (SPFLOAT)(voc->counter+0.5)/512;

tract_compute(sp,&voc->tr,*in,lambda1);
vocal_output+= voc->tr.lip_output+voc->tr.nose_output;
tract_compute(sp,&voc->tr,*in,lambda2);
vocal_output+= voc->tr.lip_output+voc->tr.nose_output;


*out= vocal_output*0.125;
voc->counter= (voc->counter+1)%512;
return SP_OK;
}

/*:13*/
#line 15 "./top.w"

/*14:*/
#line 133 "./top.w"

void sp_voc_set_frequency(sp_voc*voc,SPFLOAT freq)
{
voc->glot.freq= freq;
}

/*:14*/
#line 16 "./top.w"

/*15:*/
#line 143 "./top.w"

SPFLOAT*sp_voc_get_frequency_ptr(sp_voc*voc)
{
return&voc->glot.freq;
}

/*:15*/
#line 17 "./top.w"

/*16:*/
#line 152 "./top.w"

SPFLOAT*sp_voc_get_tract_diameters(sp_voc*voc)
{
return voc->tr.target_diameter;
}

/*:16*/
#line 18 "./top.w"

/*17:*/
#line 163 "./top.w"

SPFLOAT*sp_voc_get_current_tract_diameters(sp_voc*voc)
{
return voc->tr.diameter;
}

/*:17*/
#line 19 "./top.w"

/*18:*/
#line 170 "./top.w"

int sp_voc_get_tract_size(sp_voc*voc)
{
return voc->tr.n;
}

/*:18*/
#line 20 "./top.w"

/*19:*/
#line 177 "./top.w"


SPFLOAT*sp_voc_get_nose_diameters(sp_voc*voc)
{
return voc->tr.nose_diameter;
}

/*:19*/
#line 21 "./top.w"

/*20:*/
#line 185 "./top.w"

int sp_voc_get_nose_size(sp_voc*voc)
{
return voc->tr.nose_length;
}

/*:20*/
#line 22 "./top.w"

/*21:*/
#line 213 "./top.w"

void sp_voc_set_diameters(sp_voc*voc,
int blade_start,
int lip_start,
int tip_start,
SPFLOAT tongue_index,
SPFLOAT tongue_diameter,
SPFLOAT*diameters){

int i;
SPFLOAT t;
SPFLOAT fixed_tongue_diameter;
SPFLOAT curve;
int grid_offset= 0;

for(i= blade_start;i<lip_start;i++){
t= 1.1*M_PI*
(SPFLOAT)(tongue_index-i)/(tip_start-blade_start);
fixed_tongue_diameter= 2+(tongue_diameter-2)/1.5;
curve= (1.5-fixed_tongue_diameter+grid_offset)*cos(t);
if(i==blade_start-2||i==lip_start-1)curve*= 0.8;
if(i==blade_start||i==lip_start-2)curve*= 0.94;
diameters[i]= 1.5-curve;
}
}


/*:21*/
#line 23 "./top.w"

/*22:*/
#line 253 "./top.w"


void sp_voc_set_tongue_shape(sp_voc*voc,
SPFLOAT tongue_index,
SPFLOAT tongue_diameter){
SPFLOAT*diameters;
diameters= sp_voc_get_tract_diameters(voc);
sp_voc_set_diameters(voc,10,39,32,
tongue_index,tongue_diameter,diameters);
}

/*:22*/
#line 24 "./top.w"

/*23:*/
#line 271 "./top.w"


int sp_voc_get_counter(sp_voc*voc)
{
return voc->counter;
}
/*:23*/
#line 25 "./top.w"

/*24:*/
#line 285 "./top.w"

void sp_voc_set_tenseness(sp_voc*voc,SPFLOAT tenseness)
{
voc->glot.tenseness= tenseness;
}

/*:24*/
#line 26 "./top.w"

/*25:*/
#line 296 "./top.w"

SPFLOAT*sp_voc_get_tenseness_ptr(sp_voc*voc)
{
return&voc->glot.tenseness;
}

/*:25*/
#line 27 "./top.w"

/*26:*/
#line 306 "./top.w"

void sp_voc_set_velum(sp_voc*voc,SPFLOAT velum)
{
voc->tr.velum_target= velum;
}

/*:26*/
#line 28 "./top.w"

/*27:*/
#line 316 "./top.w"


SPFLOAT*sp_voc_get_velum_ptr(sp_voc*voc)
{
return&voc->tr.velum_target;
}

#line 115 "./voc.w"

#line 1 "./glottis.w"
/*:27*/
#line 29 "./top.w"


/*:8*/
#line 110 "./voc.w"


#line 1 "./data.w"
/*:1*/
