//
//  pdf_sound.c
//  MuPDF
//
//  Created by SuYuanCheng on 13-12-4.
//  Copyright (c) 2013年 Artifex Software, Inc. All rights reserved.
//
#include"pdf-sound.h"
#include"pdf-password.h"
#include "mupdf/fitz.h"
#include<zlib.h>
#include <stdio.h>
#include<JavaScriptCore.h>

/*
 *flash path and sound image path
 */
char *flash_path=NULL;
char *sound_image_path = NULL;

typedef struct  WAV_HEADER
{
    char                RIFF[4];
    int                 ChunkSize;
    char                WAVE[4];
    char                fmt[4];
    int                 Subchunk1Size;
    short int           AudioFormat;
    short int           NumOfChan;
    int                 SamplesPerSec;
    int                 bytesPerSec;
    short int           blockAlign;
    short int           bitsPerSample;
    char                Subchunk2ID[4];
    int                 Subchunk2Size;
   
}wav_hdr;

wav_hdr get_wav_header(char *wavfile)
{
   
    
    wav_hdr wavHeader;
//    unsigned char s[60];
    FILE *wavFile;
    int headerSize = sizeof(wav_hdr);
    wavFile = fopen(wavfile,"r");
    if(wavFile == NULL)
    {
        printf("Unable to open wave file\n");
//        exit(EXIT_FAILURE);
    }
    fread(&wavHeader,headerSize,1,wavFile);
//  fread(s,sizeof(unsigned char),60,wavFile);
    fclose(wavFile);
   
    return wavHeader;
}


void set_flash_path(char *flashPath)
{
    int len = strlen(flashPath);
    if(!len)
    {
        printf("flash path is null");
        return;
    }
    flash_path = (char *)malloc(len*(sizeof(char)));
    memcpy(flash_path, flashPath, len);
    
    return ;
}

void set_sound_image_path(char *soundImagePath)
{
    int len = strlen(soundImagePath);
    if(!len)
    {
        printf("sound image path is null");
        return;
    }
    sound_image_path = (char *)malloc(len*(sizeof(char)));
    memcpy(sound_image_path, soundImagePath, len);
}
static
int create_xobject_img(pdf_document *doc,const char *imgName,const float width,const float height)
{
    pdf_obj *imgXobject =pdf_new_dict(doc, 0);
    int img = pdf_create_object(doc);
    
    fz_try(doc->ctx)
    {
        pdf_dict_putp_drop(imgXobject, "Type", pdf_new_name(doc, "XObject"));
        pdf_dict_putp_drop(imgXobject, "Filter", pdf_new_name(doc, "DCTDecode"));
        pdf_dict_putp_drop(imgXobject, "Subtype", pdf_new_name(doc, "Image"));
        pdf_dict_putp_drop(imgXobject, "Width", pdf_new_real(doc, width));
        pdf_dict_putp_drop(imgXobject,"Height", pdf_new_real(doc, height));
        pdf_dict_putp_drop(imgXobject, "ColorSpace", pdf_new_name(doc, "DeviceRGB"));
        pdf_dict_putp_drop(imgXobject, "BitsPerComponent", pdf_new_int(doc, 8));
        
        fz_stream *imgStream =  fz_open_file(doc->ctx,imgName);
        fz_buffer *imgBuffer = fz_read_all(imgStream, 1024);
       
        pdf_dict_putp_drop(imgXobject, "Length", pdf_new_int(doc, imgBuffer->len));
        
        pdf_update_object(doc, img, imgXobject);
        pdf_update_stream(doc, img, imgBuffer);
    }
    fz_always(doc->ctx)
    {
        pdf_drop_obj(imgXobject);
    }
    fz_catch(doc->ctx)
    {
        fz_throw(doc->ctx, 1, "create imge error");
    }
    return img;
    
}
static
pdf_obj * create_extgstate(pdf_document *doc, float opacity)
{
    int idict_num;
    pdf_obj *ExtGState = NULL;
    pdf_obj *obj = NULL;
    pdf_obj *dict = NULL;
    
    fz_context *ctx = doc->ctx;
    
    fz_var(ExtGState);
    fz_var(dict);
    fz_var(obj);
    
    fz_try(ctx)
    {
        dict = pdf_new_dict(doc, 3);
        
        obj = pdf_new_real(doc, opacity);
        pdf_dict_puts(dict, "CA", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_new_name(doc, "ExtGState");
        pdf_dict_puts(dict, "Type", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        obj = pdf_new_real(doc, opacity);
        pdf_dict_puts(dict, "ca", obj);
        pdf_drop_obj(obj);
        obj = NULL;
        
        idict_num = pdf_create_object(doc);
        pdf_update_object(doc, idict_num, dict);
        ExtGState = pdf_new_indirect(doc, idict_num, 0);
        pdf_drop_obj(dict);
        dict = NULL;
    }
    fz_catch(ctx)
    {
        pdf_drop_obj(obj);
        pdf_drop_obj(dict);
        pdf_drop_obj(ExtGState);
    }
    return ExtGState;
}
/*
 创建一个form
 img:图片xobject的num
 */
static
int create_xobject_form(pdf_document *doc,int img,const fz_matrix matrix,const fz_rect *bbox)
{
    
    pdf_obj *xobject =  pdf_new_xobject(doc,bbox, &matrix);
    pdf_obj *xoo = pdf_load_object(doc, pdf_to_num(xobject), 0);//the xobject's dictionary
    
 
    //add the graphic state
    pdf_obj *res = pdf_new_dict(doc, 3);
    pdf_obj *extgstate = create_extgstate(doc, 1);
    pdf_obj * GSW = pdf_new_dict(doc, 1);
    pdf_dict_putp_drop(GSW, "GSW", extgstate);
    pdf_dict_putp_drop(res, "ExtGState", GSW);
    
    // the doc settings
    pdf_obj *procSet = pdf_new_array(doc, 2);
    pdf_array_push_drop(procSet, pdf_new_name(doc, "PDF"));
    pdf_array_push_drop(procSet, pdf_new_name(doc, "ImageC"));
    pdf_dict_putp_drop(res, "ProcSet", procSet);
    pdf_obj *ximg = pdf_new_dict(doc, 1);
    pdf_dict_putp_drop(ximg, "Img0", pdf_new_indirect(doc, img, 0));
    pdf_dict_putp_drop(res, "XObject", ximg);
    pdf_dict_putp_drop(xoo,"Resources", res);
    
    pdf_obj *imga = pdf_load_object(doc, img, 0);
    float with = pdf_to_real(pdf_dict_gets(imga,  "Width"));
    float height = pdf_to_real(pdf_dict_gets(imga, "Height"));
    
    char *data = (malloc(100*sizeof(char)));
    sprintf(data, "%s","q\n");
    
    char *w = (char *)malloc(2*sizeof(char));
    sprintf(w, "%g ",with);
    strcat(data, w);
    free(w);
    strcat(data,"0 0 ");
    
    char *h = (char *)malloc(2*sizeof(char));
    sprintf(h, "%g ",height);
    strcat(data, h);
    free(h);
    char *b = (char*)malloc(4*sizeof(char));
    sprintf(b, "%g %g ",bbox->x0,bbox->y0);//the location of the form
    strcat(data, b);
    free(b);
    
    strcat(data,"cm\n/GSW gs\n/Img0 Do\nQ\n");
    
    fz_buffer *buffer = fz_new_buffer_from_data(doc->ctx,( unsigned char *)data , strlen(data));
    pdf_update_xobject_contents(doc, pdf_load_xobject(doc, xobject), buffer);
    return pdf_to_num(xobject);
    
}

static unsigned char char_to_hex( unsigned char x )
{
    return (unsigned char)(x > 9 ? x + 55: x + 48);
}

static int is_alpha_number_char( unsigned char c )
{
    if ( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') )
        return 1;
    return 0;
}

//url编码实现
static
void urlencode( unsigned char * src, int  src_len, unsigned char * dest, int  dest_len )
{
    unsigned char ch;
    int  len = 0;
    
    while (*src)
    {
        ch = (unsigned char)*src;
        if (is_alpha_number_char(ch) || strchr("-_.!~*'()", ch))
        {
            *dest++ = *src;
        }
        else
        {
            *dest++ = '%';
            *dest++ = char_to_hex( (unsigned char)(ch >> 4) );
            *dest++ = char_to_hex( (unsigned char)(ch % 16) );
        }
        ++src;
        ++len;
    }
    *dest = 0;
    return ;
}
static
char *flashVars(char *str,int *len)
{
    char *tem = rindex(str, '/');
    char dest[200];
    tem=tem+1;
    char *url = (char *)malloc((strlen(tem)+200)*sizeof(char));
    memcpy(url, "source=", 7);
    urlencode(tem, strlen(tem), dest, 0);
    memcpy(url+7, dest, strlen(dest));
    memcpy(url+7+strlen(dest), "&autoPlay=true&volume=1.00", 26);
    *len = strlen(dest)+33;
    return url;
    
}


/*
 set the richmedia annotation to a page obj
 */
static
pdf_obj * richMedia_set_annot(pdf_document *doc, pdf_annot *annot,fz_point location)
{
    int img =0;
    int form =0;
    const fz_matrix matrix = {1,0,0,1,0,0};
     const fz_rect bbox = {0,0,64,64};
    pdf_obj *annot_obj = annot->obj;
    pdf_dict_putp_drop(annot_obj, "Subtype", pdf_new_name(doc, "RichMedia"));
    pdf_dict_putp_drop(annot_obj, "NM", pdf_new_string(doc, "richmedia001", 20));
  
    fz_rect frect;
    frect.x0 =location.x-32;
    frect.x1=location.x+32;
    frect.y0 = location.y-32;
    frect.y1 = location.y+32;
    fz_transform_rect(&frect, &annot->page->ctm);
    pdf_obj *rect = pdf_new_array(doc, 4);
    pdf_array_push_drop(rect, pdf_new_real(doc, frect.x0));
    pdf_array_push_drop(rect, pdf_new_real(doc, frect.y0));
    pdf_array_push_drop(rect, pdf_new_real(doc, frect.x1));
    pdf_array_push_drop(rect, pdf_new_real(doc, frect.y1));
    pdf_dict_putp_drop(annot_obj, "Rect", rect);
    
    pdf_obj *boder = pdf_new_array(doc, 3);
    pdf_array_push_drop(boder, pdf_new_int(doc, 0));
    pdf_array_push_drop(boder, pdf_new_int(doc, 0));
    pdf_array_push_drop(boder, pdf_new_int(doc, 0));
    pdf_dict_putp_drop(annot_obj, "Border",  boder);
    
    pdf_obj*bs = pdf_new_dict(doc, 3);
    pdf_dict_putp_drop(bs, "Type", pdf_new_name(doc,"Border"));
    pdf_dict_putp_drop(bs, "W", pdf_new_int(doc, 0));
    pdf_dict_putp_drop(bs, "S", pdf_new_name(doc,"S"));
    pdf_dict_putp_drop(annot_obj, "BS", bs);
    
   
    img = create_xobject_img(doc, sound_image_path, 64,64);
    form = create_xobject_form(doc,img,matrix,&bbox);
    pdf_dict_putp_drop(annot_obj,"AP/N",pdf_new_indirect(doc,form,0));
    
    return annot_obj;
}

/*
set the richmedia settings
 */
static
pdf_obj * richMedia_set_settings(pdf_document *doc, pdf_obj *annot_obj)
{
    int set_num = pdf_create_object(doc);
    pdf_obj *setting = pdf_new_dict(doc, 3);
    pdf_dict_putp_drop(setting, "Type", pdf_new_name(doc, "RichMediaSettings"));
    
    /*activation item*/
    pdf_dict_putp_drop(setting, "Activation/Type", pdf_new_name(doc, "RichMediaActivation"));
    pdf_dict_putp_drop(setting, "Activation/Condition", pdf_new_name(doc, "XA"));
    
    /*presentation item*/
    pdf_dict_putp_drop(setting, "Presentation/NavigationPane", pdf_new_bool(doc, 0));
    pdf_dict_putp_drop(setting, "Presentation/PassContextClick", pdf_new_bool(doc, 0));
    pdf_dict_putp_drop(setting, "Presentation/Style", pdf_new_name(doc, "Embedded"));
    pdf_dict_putp_drop(setting, "Presentation/Toolbar", pdf_new_bool(doc, 0));
    pdf_dict_putp_drop(setting, "Presentation/Transparent", pdf_new_bool(doc, 0));
    
    
    /*deactivation item*/
    pdf_dict_putp_drop(setting, "Deactivation/Type", pdf_new_name(doc, "RichMediaDeactivation"));
    pdf_dict_putp_drop(setting, "Deactivation/Condition", pdf_new_name(doc, "XD"));
    pdf_update_object(doc, set_num, setting);
    pdf_drop_obj(setting);
    
    pdf_dict_putp_drop(annot_obj, "RichMediaSettings", pdf_new_indirect(doc, set_num, 0));
    return annot_obj;
    
}



/*
 set the richmedia content
 */
static
pdf_obj * richMedia_set_content(pdf_document *doc, pdf_obj *annot_obj,char *audiofile)
{
    int configs=0;
    int config=0;
    pdf_obj *cos=NULL;
    pdf_obj *co =NULL;
    char *vars=NULL;
    
    int len = 0;
    char *raudio=rindex(audiofile,'/')+1;
    
    int length = strlen(raudio);
    fz_stream *stream=NULL;
    fz_buffer *buffer = NULL;
    
    pdf_obj *param = pdf_new_dict(doc, 2);
    pdf_dict_putp_drop(param, "Binding", pdf_new_name(doc, "Background"));
    vars = flashVars(audiofile, &len);
    pdf_dict_putp_drop(param, "FlashVars", pdf_new_string(doc, vars, len));
    free(vars);
    
    pdf_obj *asset_o = pdf_new_dict(doc,4);
    int asset = pdf_create_object(doc);
    pdf_dict_putp_drop(asset_o, "F", pdf_new_string(doc, "AudioPlayer.swf", 15));
    pdf_dict_putp_drop(asset_o, "UF", pdf_new_string(doc, "AudioPlayer.swf", 15));
    pdf_dict_putp_drop(asset_o, "Type", pdf_new_name(doc, "Filespec"));
    
    int swf = pdf_create_object(doc);
    pdf_obj *swf_o = pdf_new_dict(doc,4);//2
    stream = fz_open_file(doc->ctx,flash_path);
    buffer = fz_read_all(stream, 1024);
 
    pdf_dict_putp_drop(swf_o, "DL", pdf_new_int(doc, buffer->len));
    pdf_dict_putp_drop(swf_o, "Length", pdf_new_int(doc, buffer->len));
    pdf_dict_putp_drop(swf_o, "Subtype", pdf_new_name(doc,"application#2Fx-shockwave-flash"));
    pdf_update_stream(doc,swf , buffer);
    pdf_update_object(doc, swf, swf_o);
    pdf_dict_putp_drop(asset_o, "EF/F", pdf_new_indirect(doc, swf,0));
    pdf_update_object(doc, asset, asset_o);
    
    int instances = pdf_create_object(doc);
    pdf_obj *instances_o = pdf_new_array(doc, 1);
    pdf_obj *instance = pdf_new_dict(doc, 2);
    pdf_dict_putp_drop(instance, "Asset", pdf_new_indirect(doc, asset,0));
    pdf_dict_putp_drop(instance, "Params", pdf_new_ref(doc, param));
    pdf_array_push_drop(instances_o, pdf_new_ref(doc, instance));
    pdf_update_object(doc, instances,instances_o);
    
    configs = pdf_create_object(doc);
    cos = pdf_new_array(doc, 1);
    config = pdf_create_object(doc);
    co = pdf_new_dict(doc, 3);
    pdf_dict_putp_drop(co, "Subtype", pdf_new_name(doc, "Sound"));
    pdf_dict_putp_drop(co, "Instances", pdf_new_indirect(doc, instances, 0));
    pdf_update_object(doc, config, co);
    pdf_array_push(cos, pdf_new_indirect(doc, config, 0));
    pdf_update_object(doc, configs, cos);
   
    pdf_obj *audio_o = pdf_new_dict(doc,4);
    int audio = pdf_create_object(doc);
    pdf_dict_putp_drop(audio_o, "F", pdf_new_string(doc, raudio,length));
    pdf_dict_putp_drop(audio_o, "UF", pdf_new_string(doc, raudio,length));
    pdf_dict_putp_drop(audio_o, "Type", pdf_new_name(doc, "Filespec"));
    
    stream = fz_open_file(doc->ctx, audiofile);
    buffer = fz_read_all(stream, 1024);
    
    int aufi= pdf_create_object(doc);
    pdf_obj *aufi_o = pdf_new_dict(doc,4);//3
    pdf_dict_putp_drop(aufi_o, "DL", pdf_new_int(doc, buffer->len));
    pdf_dict_putp_drop(aufi_o, "Length", pdf_new_int(doc, buffer->len));
    pdf_dict_putp_drop(aufi_o, "Subtype", pdf_new_name(doc,"audio#2Fmpeg"));
    pdf_update_stream(doc, aufi, buffer);
    pdf_update_object(doc, aufi, aufi_o);
    pdf_dict_putp_drop(audio_o, "EF/F", pdf_new_indirect(doc, aufi,0));
    pdf_update_object(doc, audio, audio_o);
    
    
    pdf_obj *assets = pdf_new_dict(doc, 2);
    pdf_obj *names = pdf_new_array(doc, 4);
    
    pdf_array_push_drop(names, pdf_new_string(doc, "AudioPlayer.swf", 15));
    pdf_array_push_drop(names, pdf_new_indirect(doc, asset, 0));
    pdf_array_push_drop(names, pdf_new_string(doc, raudio, length));
    pdf_array_push_drop(names, pdf_new_indirect(doc, audio, 0));
    
    pdf_dict_putp_drop(assets, "Names", pdf_new_ref(doc, names));
    pdf_obj *content = pdf_new_dict(doc, 2);
    pdf_dict_putp_drop(content, "Configurations", pdf_new_indirect(doc, configs, 0));
    pdf_dict_putp_drop(content, "Assets", pdf_new_ref(doc, assets));
    
    pdf_dict_putp_drop(annot_obj, "RichMediaContent", pdf_new_ref(doc, content));
    pdf_dict_putp_drop(annot_obj, "RichMediaSettings/Activation/Configuration", pdf_new_indirect(doc, config, 0));
    fz_drop_buffer(doc->ctx, buffer);
    return annot_obj;
    
}

/*
 *add the sound as annotation and just add wav files
 */
static
pdf_obj *add_sound(pdf_document *doc,pdf_annot *annot,fz_point location,char *filename)
{
    int sound =0 ;
    pdf_obj *soundo=NULL;
    fz_stream *file = NULL;
    fz_buffer *buffer=NULL;
    pdf_obj *obj = annot->obj;
    int img =0;
    int form =0;
    const fz_matrix matrix = {1,0,0,1,0,0};
    const fz_rect bbox = {0,0,64,64};
    wav_hdr header;
    sound = pdf_create_object(doc);
    soundo = pdf_new_dict(doc, 7);
    
    header = get_wav_header(filename);
    
    /*
     just the wav format audiofile,because the other format adobe not support
     */
    if(header.RIFF[0]=='R'&& header.RIFF[1]=='I'&&header.RIFF[2]=='F'&& header.RIFF[3]=='F')
        if(1==header.AudioFormat||6==header.AudioFormat||7==header.AudioFormat)
    {
        pdf_dict_putp_drop(soundo, "B", pdf_new_int(doc, header.bitsPerSample));
        pdf_dict_putp_drop(soundo, "C", pdf_new_int(doc, header.NumOfChan));
        switch(header.AudioFormat )  {
            case 1:    //pcm encoding
                if(8==header.bitsPerSample)
                    pdf_dict_putp_drop(soundo, "E", pdf_new_name(doc, "Raw"));
                else
                    pdf_dict_putp_drop(soundo, "E", pdf_new_name(doc, "Signed"));
                break;
            case 6:   //a law encoding
                pdf_dict_putp_drop(soundo, "E", pdf_new_name(doc, "ALaw"));
                break;
            case 7:  //mu law encoding
                pdf_dict_putp_drop(soundo, "E", pdf_new_name(doc, "muLaw"));
                break;
            default:  //no support other encoding
                pdf_drop_obj(soundo);
                printf("\nnot support this format\n");
                return obj;
                
        }
        pdf_dict_putp_drop(soundo, "R", pdf_new_int(doc, header.SamplesPerSec));
    }
    pdf_dict_putp_drop(soundo, "Filter", pdf_new_name(doc, "FlateDecode"));
    pdf_dict_putp_drop(soundo, "Type", pdf_new_name(doc, "Sound"));
    pdf_update_object(doc, sound, soundo);
    fz_rect frect;
    frect.x0 =location.x-32;
    frect.x1=location.x+32;
    frect.y0 = location.y-32;
    frect.y1 = location.y+32;
    fz_transform_rect(&frect, &annot->page->ctm);
    pdf_obj *rect = pdf_new_array(doc, 4);
    pdf_array_push_drop(rect, pdf_new_real(doc, frect.x0));
    pdf_array_push_drop(rect, pdf_new_real(doc, frect.y0));
    pdf_array_push_drop(rect, pdf_new_real(doc, frect.x1));
    pdf_array_push_drop(rect, pdf_new_real(doc, frect.y1));
    pdf_dict_putp_drop(obj, "Rect", rect);
    
   
    file =fz_open_file(doc->ctx, filename);
    buffer = fz_read_all(file, 1024);
//    *data = (unsigned char*)malloc( buffer->len*sizeof(unsigned char));
//    memcpy(*data, buffer->data, buffer->len);
//    *length = buffer->len;
    
     /*
     write the audio file stream to pdf
     */
    
    /*
     write all data to pdf if the audio file if it is not the wav format,in this way the abode cannot play it
     */
   if(header.RIFF[0]=='R'&& header.RIFF[1]=='I'&&header.RIFF[2]=='F'&& header.RIFF[3]=='F')
    if(1==header.AudioFormat||6==header.AudioFormat||7==header.AudioFormat)
    {
        buffer->data+=43;
        buffer->len-=43;
        buffer->cap = buffer->len;
    }
    compressStream(buffer, doc->ctx);
    pdf_update_stream(doc, sound, buffer);
    pdf_dict_putp_drop(soundo, "Length", pdf_new_int(doc, buffer->len));
    
    /*
     draw the picture
     */
    img = create_xobject_img(doc, sound_image_path, 64,64);
    form = create_xobject_form(doc,img,matrix,&bbox);
    pdf_dict_putp_drop(obj,"AP/N",pdf_new_indirect(doc,form,0));
    pdf_dict_putp_drop(obj, "Sound", pdf_new_indirect(doc, sound, 0));
    
    return obj;
  
   
}


/*
 *add a sound rich media to a page
 *param doc: the document which the annot be added to
 *param page: the page number
 *param filename: the rich media source name
 */
pdf_document* pdf_add_sound(pdf_document *doc,int page,fz_point location,char *filename)
{
    pdf_annot *sound_annot=NULL;
    pdf_obj *annot_obj=NULL;
    pdf_obj *tmpObj=NULL;
    char *tem = rindex(filename,'.');
    tem=tem+1;
  /*
   * method 1:
   */
    if(!strcmp(tem, "wav"))
    {
        pdf_page *page_obj = pdf_load_page(doc, page);
        sound_annot = pdf_create_annot(doc, page_obj, FZ_ANNOT_SOUND);
        add_sound(doc,sound_annot,location,filename);
    }
    
////////////////////////////////////////////////////////////////
  /*
   *method 2:
   */
    else if(!(strcmp(tem, "mp3")))
    {
        pdf_page *page_obj = pdf_load_page(doc, page);
        sound_annot = pdf_create_annot(doc, page_obj, FZ_ANNOT_SOUND);
        annot_obj = sound_annot->obj;
        /*
         step 1:add the rich media to the page obj
         */
        richMedia_set_annot(doc,sound_annot,location);
        
        /*
         step 2:set the richmedia setting
         */
        richMedia_set_settings(doc, annot_obj);
        
        /*
         step 3:set the richmedia content
         */
        richMedia_set_content(doc, annot_obj,filename);
    }
    return doc;
}
/*
 *get the sound annotation
 *param page: the page which to check
 *param point: the point get from tap event, the point is tranfered to pdf space
 */
pdf_annot *pdf_get_sound_annot(pdf_document *doc,int page,fz_point point)
{
    pdf_annot *annots=NULL;
    pdf_page *page_obj =pdf_load_page(doc, page);
    fz_point temp_point = point;
    for(annots = page_obj->annots;annots;annots=annots->next)
    {
        point =temp_point;
        int s = FZ_ANNOT_RICHMEDIA;
        if(FZ_ANNOT_SOUND !=pdf_annot_type(annots)&&FZ_ANNOT_RICHMEDIA!=pdf_annot_type(annots))
            continue;
        fz_rect *rect = &annots->rect;
        fz_transform_point(&point, &annots->page->ctm);
       if(point.x>rect->x0)
       if(point.x<rect->x1)
       if(point.y>rect->y0)
       if(point.y<rect->y1)
            return annots;
    }
    return NULL;
    
}
/*get the sound file data, and use the data to play in objective-C
 *param page: the annot belongs to the page
 *param sound: ensure the annotation is sound annotation
 */
unsigned char *pdf_get_sound_data(pdf_document *doc,pdf_annot *sound,int *length)
{
    if(sound ==NULL||(sound->annot_type != FZ_ANNOT_SOUND&&sound->annot_type != FZ_ANNOT_RICHMEDIA))
    {
        printf("not sound annotation");
        return NULL;
    }
    
    wav_hdr header;
    pdf_obj *annot_obj = sound->obj;
    pdf_obj *subtype = pdf_dict_getp(annot_obj, "Subtype");
    unsigned char *data = NULL;
    
    if(!strcmp(pdf_to_name(subtype), "Sound"))
    {
        pdf_obj *sound_obj = pdf_dict_getp(annot_obj, "Sound");
        int  num = pdf_to_num(sound_obj);
        fz_stream *stream=NULL;
        fz_buffer *buf = NULL;
        if(!pdf_is_stream(doc, num, 0))
        {
            return  NULL;
        }
        stream = pdf_open_stream( doc,num, 0);
        buf = fz_read_all(stream, 1024);
        
        header.bitsPerSample=pdf_to_int(pdf_dict_getp(sound_obj, "B"));
        
        /*
         deal with the wav format audio file
         */
        if(header.bitsPerSample !=0)
        {
            header.RIFF[0]='R'; header.RIFF[1]='I'; header.RIFF[2]='F'; header.RIFF[3]='F';
            header.fmt[0]='f';header.fmt[1]='m';header.fmt[2]='t';header.fmt[3]=' ';
            header.WAVE[0]='W';header.WAVE[1]='A';header.WAVE[2]='V';header.WAVE[3]='E';
            header.Subchunk2ID[0]='d';header.Subchunk2ID[1]='a';header.Subchunk2ID[2]='t';header.Subchunk2ID[3]='a';
            
            header.NumOfChan = pdf_to_int(pdf_dict_getp(sound_obj, "C"));
            header.SamplesPerSec=pdf_to_int(pdf_dict_getp(sound_obj, "R"));
            header.blockAlign=header.NumOfChan*header.bitsPerSample/8;
            header.bytesPerSec=header.SamplesPerSec*header.NumOfChan*header.bitsPerSample/8;
            header.Subchunk1Size = 16;
            header.Subchunk2Size = buf->len;
            header.ChunkSize=36+header.Subchunk2Size;
            if(!strcmp("Signed", pdf_to_name(pdf_dict_getp(sound_obj, "E"))))
                header.AudioFormat =1;
            if(!strcmp("Raw", pdf_to_name(pdf_dict_getp(sound_obj, "E"))))
                header.AudioFormat =1;
            if(!strcmp("ALaw", pdf_to_name(pdf_dict_getp(sound_obj, "E"))))
                header.AudioFormat =6;
            if(!strcmp("muLaw", pdf_to_name(pdf_dict_getp(sound_obj, "E"))))
                header.AudioFormat =7;
            data = (unsigned char *)malloc((43+buf->len)*sizeof(unsigned char));
            memcpy(data, &header, sizeof(wav_hdr));
            *length = buf->len+43;
            memcpy(data+sizeof(wav_hdr)-1, buf->data, buf->len);
        }
        /*
         other format audio file，such as mp3 return the all audio file back
         */
        else
        {
            data = (unsigned char *)malloc((buf->len)*sizeof(unsigned char));
            memcpy(data, buf->data, buf->len);
            *length =buf->len;
            
        }
    }
    else if(!strcmp(pdf_to_name(subtype), "RichMedia"))
    {
        pdf_obj *as = pdf_dict_getp(annot_obj, "RichMediaContent/Assets/Names");
        int le=1;
        pdf_obj *ef = pdf_array_get(as,0 );
        pdf_obj *aufile =NULL;
        fz_stream *stream=NULL;
        fz_buffer *buf = NULL;
        while (ef) {
            ef = pdf_array_get(as,le );
            le++;
        }
        ef = pdf_array_get(as, le-2);
        aufile = pdf_dict_getp(ef, "EF/F");
        stream = pdf_open_stream(doc, pdf_to_num(aufile), 0);
        buf = fz_read_all( stream, 1024);
        data = (unsigned char *)malloc(buf->len*sizeof(unsigned char));
        memcpy(data, buf->data, buf->len);
        *length = buf->len;
    }
    return data;
    
}

