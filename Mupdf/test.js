function submitAction()
{
    
}


function ppdfOnInitialize()
{
    g_preferences = p_alertonunlock | p_showpassword | p_alertonfailedselfunlock;
}





var x="2010-10-15 (Version 2.6.0 Build 2.0.7.36)";
var q="https://ppdf.phoenix.edu/ProtectedPdf/v2";
var ao=1000*60*60*24;
var ec= false;
var ew=6;


var bg=0;
var cq=false;
var dj=null;


var dn=false;
var df=false;
var ds=null;
var dw='WHATAREYOULOOKINAT?';
var ea=false;
var fw=false;
var fv=true;
var gi=null;
var FA=false;

var g_field_name_tree=new Object();

g_field_name_tree["boom.form.response"]=new Array("hash","username","docid");


var g_preferences=0xFFFFFFFF;


var p_pingonstartup=0x00000001;

var p_alertonunlock=0x00000002;

var p_alertonnewguid=0x00000004;

var p_alertofflinedays=0x00000008;
var p_usechallengeresponse=0x00000010;

var p_showpassword=0x00000020;

var p_alertonfailedselfunlock=0x00000040;

var p_showphonetemplateonstartup=0x00000080;

var p_alertonphonetemplate=0x00000100;


var p_phoneunlockinstructions="Offline Unlock Procedures:\n\n1.Please call this toll free number to unlock your document without internet access: 666.666.6666\n\n2.Provide the serial number, access code and document number.\n\n3.You will be provided a unlock code.\n\n4.Enter the unlock code and click on 'Offline Unlock' button.\n\nMessage code: 012";


var Q=/^(NotAllowedError)+(.)+/i;

var zA=/^(Document.submitForm)+/i;


var gh="_SI";
var gq="2_5_3";

var N=
{
K:"MAC"
};

var A=
{
bx:0,
JA:1,
HA:2,
a:3,
LA:4,
dy:5
};

var MA=
{
UA:0,
TA:1,
ZA:2
}

var GA= 2;

var EA=false;


var pA=25;

var R=
{
BA:"$$0_g_403356",
hA:"$$0_k_",
fA:"",
b:"$$0_o_403356",
S:"boom_cookie1000_0001_C359_0006279C_"
};

function cv()
{
    return T(R.b)!=null
    ||T(R.S+"_J")!=null;
}


function fk(message,title)
{
    title="Protectedpdf - "+((title==null)?"Message":title);
    app.alert(gd(message),3,0,title);
}


function fj(message,title)
{
    title="Protectedpdf - "+((title==null)?"Error":title);
    app.alert(gd(message),0,0,title);
}


function fl(message,title)
{
    title="Protectedpdf - "+((title==null)?"Message":title);
    return app.alert(gd(message),2,2,title)==4;
}

function gd(aa)
{
    
    if(aa.indexOf("@guid@")!=-1)
        aa=aa.replace("@guid@",U());
    
    
    if(aa.indexOf("@offlineDays@")!=-1)
        aa=aa.replace("@offlineDays@",(X.nA()!=undefined)?(X.nA()):"Unavailable");
    return aa;
}

function am()
{
    try
    {
        if(typeof ppdfOnInitialize=="function")
            ppdfOnInitialize();
        
        
        n();
        
        
        
        U(dp!="ppdfcrm");
        
        var ah=this.getField("boom.protectedpdf_version");
        
        ah.value=x;
    }
    finally
    {
        this.dirty=false;
    }
}

function cA(show)
{
    
    var eo=this.getField("boom.form.online");
    if(eo!=undefined)
        eo.display=show?display.visible:display.hidden;
    else
    {
        
        var eA=this.getField("boom.form.pwd_label");
        if(eA!=undefined)
            eA.display=show?display.visible:display.hidden;
        
        var dA=this.getField("boom.form.neversend.pwd");
        if(dA!=undefined)
            dA.display=show?display.visible:display.hidden;
        
        var bA=this.getField("boom.form.login");
        if(bA!=undefined)
            bA.display=show?display.visible:display.hidden;
    }
}

function eg()
{
    cA(true)
    
    var aA=this.getField("boom.form.offline");
    if(aA!=undefined)
        aA.display=display.hidden;
    aA.value="";
    
    var isNoInternet=this.getField("boom.form.offline_ckb");
    isNoInternet.checkThisBox(0,false);
}

function ef()
{
    cA(false);
    
    var aA=this.getField("boom.form.offline");
    aA.display=display.visible;
    
    var isNoInternet=this.getField("boom.form.offline_ckb");
    isNoInternet.checkThisBox(0,true);
}

function ww()
{
    return "8b8214c0-7074-4fca-a43b-cedb62895ef4";
}

function wb()
{
    return "edcf16b6-9568-4ca7-9c70-b1ac635457e2";
}

function u()
{
    try
    {
        var info=this.getField("boom.info");
        info.display=display.hidden;
        
        info.display=display.visible;

        var ad=this.getField("boom.form");
        ad.display=display.visible;
        
        if(this.getField("boom.form.online.unlock")!=null)
        {
            if(this.getField("boom.form.response.username")!=null)
                eg();
            else
                fj("There is a problem with this protectedpdf document. The template may be damaged. Please contact the publisher for a clean file.\n\nMessage code: 014");
        }
        else if(this.getField("boom.form.offline.unlock")!=null)
        {
            
            eg();
            if(this.getField("boom.form.offline.ac")!=null&&this.getField("boom.form.offline.uc")!=null&&this.getField("boom.form.response.username")!=null)
            {
                if(dx(p_showphonetemplateonstartup))
                    ef();
            }
            else
                fj("There is a problem with this protectedpdf document. The template may be damaged. Please contact the publisher for a clean file.\n\nMessage code: 015");
        }
        
        var s=this.getField("boom.unlocked");
        if(s!=null)
            s.display=display.hidden;
        
        var z=this.getField("boom.form.response");
        z.value="";
        
        var dc=this.getOCGs();
        for(w=0;w<dc.length;w++)
        {
            if(dc[w].name==wb())
            {
                dc[w].state=false;
            }
            if(dc[w].name==ww())
            {
                dc[w].state=true;
            }
        }
        
        
        as(1);
        
        df=false;
    }
    catch(cr)
    {
        fk("There is a problem with this protectedpdf document. It may be damaged or has been tampered with. Please contact the publisher for a clean file.\n\nMessage code: 013");
    }
    finally
    {
        this.dirty=false;
    }
}

function r()
{
    try
    {
        
        this.delay=true;
        
        var info=this.getField("boom.info");
        info.display=display.hidden;
        
        var ad=this.getField("boom.form");
        ad.display=display.hidden;
        
        
        ad=this.getField("boom.form.response.docid");
        if(ad!=null)
            ad.display=display.visible;
        ad=this.getField("boom.form.docid_label");
        if(ad!=null)
            ad.display=display.visible;
        
        var s=this.getField("boom.unlocked");
        if(s!=null)
            s.display=display.hidden;
       
        var dc=this.getOCGs();
        
        for(w=0;w<dc.length;w++)
        {
            //return dc[w].name;
            if(dc[w].name==wb())
            {
                dc[w].state=true;
                //return dc[w].state;
            }
            if(dc[w].name==ww())
            {
                dc[w].state=false;
               // return dc[w].state;

            }
        }        
        
        as(0);
        
      //  return "donw";
        this.delay=false;
        
        df=true;
        
        
        var gg=false;
        
        if(!fm&&!fw)
        {
            
            if(dx(p_alertonunlock)&&(fl("The document has been successfully unlocked. Would you lik to be automatically signed into this document next time? Please click No if this is a public computer.\n\nMessage code: 001")))
            {
                CA();
            }
            else if(!dx(p_alertonunlock))
            {
                CA();
            }
            
            gg=true;
        }
        
        if(fm)
            gg=true;
        
        if(fw&&dx(p_alertonunlock))
            fk("The document has been successfully unlocked.\n\nMessage code: 003");
        
        
        var gk=this.getField("boom.unlocked.rememberme");
        var gj=this.getField("boom.unlocked.rememberme_label");
        
        if(gk!=null)
            gk.display=gg?display.visible:display.hidden;
        
        if(gj!=null)
            gj.display=gg?display.visible:display.hidden;
        
        if(typeof ppdfOnUnlock=="function")
            ppdfOnUnlock();
    }
    finally
    {
        this.dirty=false;
    }
}



function ppdfRefreshDocument()
{
    if(df)
        r();
    else
        u();
}

function dh(YA)
{
    
    var dr="";
    
    if(T(R.S+"_J")!=null)
        dr=T(R.S+"_J");
    
    
    if(X!=undefined&&X.username!=null)
        dr=X.username;
    
    var d=this.getField("boom.form.response.username");
    
    if(d!=null)
        d.value=dr;
    
    
    Y(R.S+"_Y");
    Y(R.S+"_G");
    Y(R.S+"_K");
    Y(R.S+"_J");
    Y(R.S+"_L");
    Y(R.S+"_H");
    
    if(YA)
        Y(iA());
    
    var c=this.getField("boom.unlocked.rememberme");
    if(c!=null)
        c.value='Off';
}

function bx(gp)
{
    try
    {
        
        var dr="";
        
        
        if(X!=undefined&&X.username!=null)
            dr=X.username;
        
        var d=this.getField("boom.form.response.username");
        
        if(d!=null)
            d.value=dr;
        
        
        var m=["boom.form.response","boom.version","boom.hidden.gen","boom.session"];
        
        
        var ay=U(dp!="ppdfcrm");
        
        
        
        
        if(ay!=""||dp=="ppdfcrm")
            C(A.bx,m,gp);
        
        if(d!=null)
            d.value="";
    }
    catch(cr)
    {
        
        if(xy(cr.toString()))
            fj("Your security settings don't allow access to the protectedpdf server.  You must allow access to the server.\n\nMessage code: 057");
        else
            fj("There is a problem with this protectedpdf document. It may be damaged. Please contact the publisher for a clean file.\n\nMessage code: 069");
    }
    finally
    {
        this.dirty=false;
    }
}

function xY()
{
    return app.plugIns==undefined;
}

function xy(exception)
{
    return((xY()&&zA.test(exception))||Q.test(exception));
}

function J(gp,z)
{
    var O=new Object();
    
    var ay=U(dp!="ppdfcrm");
    var gs=cv();
    
    O["g"]=escape(ay);
    O["s"]=escape(gp);
    O["v"]=escape(app.viewerVersion);
    O["t"]=escape(app.viewerType);
    O["w"]=escape(gs);
    O["j"]=escape(gq);
    O["wuo"]=escape(fm);
    
    if(z!=undefined)
        B(z,O);
    
    return O;
}

function H(formfields)
{
    var O=new Object();
    
    var ay=U(true);
    
    O["g"]=escape(ay);
    O["sso"]=escape(dn);
    O["v"]=escape(app.viewerVersion);
    O["t"]=escape(app.viewerType);
    O["j"]=escape(gq);
    
    if(formfields!=undefined)
        B(formfields,O);
    
    return O;
}

function G(z)
{
    var O=new Object();
    
    var ay=U(true);
    var h=this.getField("boom.form.neversend.pwd");
    
    O["g"]=escape(ay);
    O["sso"]=escape(dn);
    O["qe"]=gt(h.value.toString());
    O["v"]=escape(app.viewerVersion);
    O["t"]=escape(app.viewerType);
    O["j"]=escape(gq);
    
    if(z!=undefined)
        B(z,O);
    
    return O;
}

function I(hash,z)
{
    var O=new Object();
    
    O["h"]=escape(hash);
    O["sso"]=escape(dn);
    O["v"]=escape(app.viewerVersion);
    O["t"]=escape(app.viewerType);
    O["j"]=escape(gq);
    
    if(z!=undefined)
        B(z,O);
    
    return O;
}

function F(z)
{
    var O=new Object();
    
    var ay=U(true);
    
    O["g"]=escape(ay);
    
    if(z!=undefined)
        B(z,O);
    
    return O;
}

function D(hash,z)
{
    var O=new Object();
    O["h"]=escape(hash);
    O["j"]=escape(gq);
    
    if(z!=undefined)
        B(z,O);
    
    return O;
}

function B(z,O)
{
    var du;
    for(w in z)
    {
        if(g_field_name_tree[z[w]]!=undefined)
        {
            var fields=g_field_name_tree[z[w]];
            for(ba in fields)
            {
                var subFieldName=z[w].toString()+"."+fields[ba].toString();
                du=this.getField(subFieldName);
                if(du!=null)
                    O[subFieldName]=escape(du.value);
            }
        }
        else
        {
            du=this.getField(z[w].toString());
            if(du!=null)
                O[z[w].toString()]=escape(du.value);
        }
    }
}





function C(action,z,gp,hash)
{
    var lA=false;
    if(L()==N.K&&app.viewerVersion>=9)
        lA=true;
    
    var O;
    var subUrl;
    switch(action)
    {
        case A.bx:
            O=lA?J(gp,z):J(gp);
            subUrl="post/ping.aspx";
            break;
        case A.JA:
            O=lA?H(z):H();
            subUrl="post/login.aspx";
            break;
        case A.HA:
            O=lA?G(z):G();
            subUrl="post/direct_login.aspx";
            break;
        case A.a:
            O=lA?I(hash,z):I(hash);
            subUrl="post/response.aspx";
            break;
        case A.LA:
            O=lA?F(z):F();
            subUrl="post/move_request.aspx";
            break;
        case A.dy:
            O=lA?D(hash,z):D(hash);
            subUrl="post/move_response.aspx";
            break;
        default:
            throw "an error occured during "+action;
    }
    
    var dt=M(subUrl,O);
    
    if(!lA)
        this.submitForm(dt,false,false,z);
    else
        this.submitForm(dt);
    
}

function M(subUrl,O)
{
    var P="";
    var dt=q+subUrl;
    if(O!=undefined)
    {
        for(w in O)
            P+=w.toString()+"="+O[w]+"&";
        if(P!=undefined&&P[P.length-1]=="&")
            P=P.substr(0,P.length-1);
        
        dt+="?"+P+"#FDF";
    }
    
    return dt;
}

function L()
{
    return app.platform;
}

function dx(bit)
{
    if(typeof(bit)!="number")
        return;
    
    return(bit&g_preferences)
}


function as(hide)//handle the 3d show
{
    
    if(!(app.viewerVersion<7))
    {
        
        for(w=0;w<this.numPages;w++)
        {
            var fu=this.getAnnots3D(w);
            if(typeof fu!='undefined')
            {
                for(ba=0;ba<fu.length;ba++)
                {
                    if(hide)
                        fu[ba].activated=false;
                    else
                        fu[ba].activated=true;
                }
            }
        }
    }
}

function fx()
{
    if(typeof ppdfOnJumpToLogin!='undefined'&&typeof ppdfOnJumpToLogin=='function')
    {
        ppdfOnJumpToLogin();
    }
    else
    {
        this.pageNum= 0;
    }
}

function CA()
{
    if(!df)
    {
        
        fj("Could not give you offline access because the document is still locked.\n\nMessage code: 006");
        return;
    }
    
    if(X!=undefined)
    {
        
        var l=new Date();
        var vitrV=new Date(l.getUTCFullYear(),l.getUTCMonth(),l.getUTCDate(),0,0,0);
        X.ak=((fv?vitrV.getTime():l.getTime())+(ao*(bg+1)));
        
        X.lastDays=X.nA()-1;
        
        if(ec&&dj!=null)
            X.watermarkText=new String(dj);
        
        X.save();
    }
    else
    {
        var offlineCookie=new AA();
        
        var d=this.getField("boom.form.response.username");
        if(d!=undefined)
            ds=new String(IA(d.value));
        if(ds!=null&&ds!="")
            offlineCookie.username=new String(ds);
        
        var l=new Date();
        var vitrV=new Date(l.getUTCFullYear(),l.getUTCMonth(),l.getUTCDate(),0,0,0);
        offlineCookie.ak=((fv?vitrV.getTime():l.getTime())+(ao*(bg+1)));
        
        if(ec&&dj!=null)
            offlineCookie.watermarkText=new String(dj);
        
        offlineCookie.i=dw;
        
        offlineCookie.save();
    }
    
    var c=this.getField("boom.unlocked.rememberme");
    if(c!=null)
        c.value='Yes';
}

var f=";";
var X;

function AA(Z,username,ak,lastDays,watermarkText,i)
{
    this.Z=Z;
    this.username=username;
    this.ak=ak;
    this.lastDays=lastDays;
    this.watermarkText=watermarkText;
    this.i=i;
    this.qA=function()
    {
        var aa=this.toString();
        return bu(this.i,aa);
    }
    this.nA=function()
    {
        if(this.ak==null||this.ak=="")
            return undefined;
        
        var l=new Date();
        var vitrV=new Date(l.getUTCFullYear(),l.getUTCMonth(),l.getUTCDate(),l.getUTCHours(),l.getUTCMinutes(),l.getUTCSeconds());
        var aj=(this.ak*1-(fv?vitrV.getTime():l.getTime()))/ao;
        aj=Math.floor(aj)
        
        
        bg=aj+1;
        return aj+1;
    };
    
    this.isExpired=function(useUTC)
    {
        var l=new Date();
        var vitrV=new Date(l.getUTCFullYear(),l.getUTCMonth(),l.getUTCDate(),l.getUTCHours(),l.getUTCMinutes(),l.getUTCSeconds());
        
        if((useUTC?vitrV.getTime():l.getTime())>this.ak)
            return true;
        
        return false;
    };
    
    this.isTampered=function()
    {
        if(this.Z!=undefined&&this.Z!="")
        {
            if(this.Z!=this.qA())
                return true;
        }
        
        if(this.lastDays!=undefined&&this.lastDays!="")
        {
            
            if((this.nA()-1)>this.lastDays)
                
                return true;
        }
        
        return false;
    };
    
    this.save=function()
    {
        if(R.fA!="")
            DA(R.hA+R.fA,this.qA()+this.toString(),true);
        else
            DA(R.b,this.qA()+this.toString(),true);
    };
    
    this.toString=function()
    {
        var aa=f;
        aa+=this.username==null?"":escape(this.username);
        aa+=f;
        aa+=this.ak==null?"":this.ak;
        aa+=f;
        aa+=this.lastDays==null?"":this.lastDays;
        aa+=f;
        aa+=this.watermarkText==null?"":this.watermarkText;
        return aa;
    };
}

function T(W)
{
    try
    {
        var script="global."+W;
        return eval(script);
    }
    catch(cr)
    {
        if(mA(cr.toString()))
        {
            if(GA==MA.UA)
                kA("Error occurred when get cookie. Invalid security message mode. Exception: "+cr.toString());
            
            return rA(W);
        }
        
        if(U()=="")
            kA("Error occurred when get cookie. Machine Id not found. Exception: "+cr.toString());
        else
            fj("There is a problem with this protectedpdf document. It may be damaged. Please contact the publisher for a clean file.\n\nMessage code: 059");
    }
}

function rA(W)
{
    var w=0;
    var script="";
    var jA="";
    
    while(w<pA)
    {
        jA=W+"_"+w.toString();
        script="global."+jA;
        try
        {
            
            sA(200);
            return eval(script);
        }
        catch(cr)
        {
            if(mA(cr.toString()))
                w++;
            else
            {
                kA("Error occurred when get cookie: cookie name: "+script+", exception: "+cr.toString());
                break;
            }
        }
    }
}

function DA(W,val,overriteOld)
{
    try
    {
        
        var script="if (global."+W+"== null || overriteOld){global."+W+" = new Object(); global.setPersistent('"+W+"', true);global."+W+" = (typeof val != 'string' ? val : '"+val+"');}";
        eval(script);
    }
    catch(cr)
    {
        if(mA(cr.toString()))
        {
            if(GA==MA.UA)
                kA("Error occurred when save cookie. Invalid security message mode. Exception: "+cr.toString());
            
            tA(W,val,overriteOld);
            return;
        }
        
        if(U()=="")
            kA("Error occurred when save cookie. Machine Id not found. Exception: "+cr.toString());
        else
            fj("There is a problem with this protectedpdf document. It may be damaged. Please contact the publisher for a clean file.\n\nMessage code: 058");
    }
}

function tA(W,val,overriteOld)
{
    var w=0;
    var script="";
    var jA="";
    
    while(w<pA)
    {
        try
        {
            jA=W+"_"+w.toString();
            script="if (global."+jA+"== null || overriteOld){global."+jA+" = new Object(); global.setPersistent('"+jA+"', true);global."+jA+" = (typeof val != 'string' ? val : '"+val+"');}";
            eval(script);
            break;
        }
        catch(cr)
        {
            if(mA(cr.toString()))
                w++;
            else
            {
                kA("Error occurred when save cookie: cookie name: "+jA+", exception: "+cr.toString());
                break;
            }
        }
    }
}

function Y(W)
{
    try
    {
        
        eval("if (global."+W+" != null){delete global."+W+";}");
        if(!EA)
            oA(W);
    }
    catch(cr)
    {
        
        if(mA(cr.toString()))
        {
            if(GA==MA.UA)
                kA("Error occurred when delete cookie. Invalid security message mode. Exception: "+cr.toString());
            
            oA(W);
            return;
        }
        
        if(U()=="")
            kA("Error occurred when delete cookie. Machine Id not found. Exception: "+cr.toString());
        else
            fj("There is a problem with this protectedpdf document. It may be damaged. Please contact the publisher for a clean file.\n\nMessage code: 062");
    }
}

function oA(W)
{
    var w=0;
    var script="";
    var jA="";
    
    while(w<pA)
    {
        try
        {
            jA=W+"_"+w.toString();
            eval("if (global."+jA+" != null){delete global."+jA+";}");
            
            w++;
        }
        catch(cr)
        {
            if(mA(cr.toString()))
                w++;
            else
            {
                kA("Error occurred when delete cookie: cookie name: "+jA+", exception: "+cr.toString());
                break;
            }
        }
    }
}

function QA(offlineCookie)
{
    try
    {
        if(offlineCookie!=null||offlineCookie!="")
        {
            var splitedCookie=offlineCookie.split(f);
            if(splitedCookie.length!=5)
                throw "incorrect offline cookie length";
            
            return new AA(splitedCookie[0],unescape(splitedCookie[1]),splitedCookie[2],splitedCookie[3],splitedCookie[4],dw);
        }
    }
    catch(cr)
    {
        
        return null;
    }
}

function PA(ak,useUTC)
{
    var l=new Date();
    var vitrV=new Date(l.getUTCFullYear(),l.getUTCMonth(),l.getUTCDate(),l.getUTCHours(),l.getUTCMinutes(),l.getUTCSeconds());
    
    if((useUTC?vitrV.getTime():l.getTime())>ak)
    {
        
        dh(true);
        fj("Your offline access has expired. You need to re-login.\n\nMessage code: 007","Message");
        
        return true;
    }
    
    return false;
}

function OA()
{
    try
    {
        var gm=false;
        var gA=iA();
        
        if(T(gA)!=undefined)
        {
            var offlineCookie=QA(T(gA));
            if(offlineCookie!=null)
            {
                if(offlineCookie.Z=="")
                {
                    dh(true);
                    return;
                }
                
                if(offlineCookie.isTampered())
                {
                    dh(true);
                    return;
                }
                
                if(offlineCookie.isExpired(fv))
                {
                    
                    dh(true);
                    fj("Your offline access has expired. You need to re-login.\n\nMessage code: 007","Message");
                    return;
                }
                
                if(ec)
                {
                    if(offlineCookie.watermarkText!="")
                        dj=offlineCookie.watermarkText;
                    else
                        return;
                }
                
                offlineCookie.lastDays=offlineCookie.nA()-1;
                offlineCookie.save();
                
                X=offlineCookie;
                
                gm=true;
                return;
            }
            
            return;
        }
        
        if(T(R.S+"_K")==null)
        {
            dh(true);
            return;
        }
        if(T(R.S+"_K")!=bu(dw,"1000_0001_C359_0006279C"))
        {
            dh(true);
            return;
        }
        if(T(R.S+"_Y")==undefined)
            return;
        
        var ak=T(R.S+"_Y");
        
        if(T(R.S+"_L")!=undefined)
            fv=T(R.S+"_L");
        
        if(PA(ak,fv))
            return;
        
        if(ec)
        {
            if(T(R.S+"_H")==undefined||T(R.S+"_H")=="")
                return;
        }
        
        gm=true;
    }
    finally
    {
        var c=this.getField("boom.unlocked.rememberme");
        if(c!=null)
            c.value=gm?'Yes':'Off';
        
        return gm;
    }
}

function KA()
{
    try
    {
        var yA=OA();
        
        if(yA&&X==undefined)
        {
            
            X=new AA(T(R.S+"_K"),
                     T(R.S+"_J"),
                     T(R.S+"_Y"),
                     T(R.S+"_G"),
                     T(R.S+"_H"),
                     dw);
            
            
            if(R.fA=="")
                dh(false);
        }
        
        return yA;
    }
    catch(cr)
    {
        return false;
    }
    finally
    {
        this.dirty=false;
    }
}

function WA()
{
    try
    {
        var ay="{"+new String((new Date()).valueOf())+"-";
        for(var w=1;w<=32;w++)
        {
            var cx=Math.floor(Math.random()*16.0).toString(16);
            ay+=cx;
            if((w==8)||(w==12)||(w==16)||(w==20))
                ay+="-";
        }
        ay+="}";
        
        
        DA(R.BA,new String(ay),true);
        
        if(dx(p_alertonnewguid))
            fk("Welcome to your first protectedpdf document. Protectedpdf documents are PDF files that require authorization from the publisher prior to access. Some documents may only be opened on a limited number of machines. Please visit www.vitrium.com to learn more.\n\nThis machine is: @guid@\n\nMessage code: 004");
        console.println("New GUID generated for this machine.");
        
        return ay;
    }
    catch(cr)
    {
        throw "Guid creation error: "+cr;
    }
}

function U(go)
{
    try
    {
        
        
        
        if(gi!=null&&gi!=gh)
            return gi;
        
        
        if(gi==gh)
            return "error: 1";
        
        gi=gh;
        
        
        var oldGuid;
        
        var newGuid;
        try
        {
            
            
            newGuid=eval("global."+R.BA);
            
            
            oldGuid=global.boom_guid;
            
        }
        catch(cr)
        {
            EA=mA(cr.toString());
            
            if(go!=null&&go)
            {
                if(EA)
                {
                    if(GA==MA.UA)
                    {
                        fj("This document requires global security policy to be disabled.\n\nPlease go to Edit > Preferences > JavaScript and uncheck the \"Enable global object security policy\" checkbox. NOTE: In some versions of Adobe Reader you may need to enable JavaScript first.\n\nMessage code: 005");
                        fj("The document will now close.\n\nMessage code: 064");
                        this.closeDoc(true);
                        FA=true;
                        return "error: 2";
                    }
                    
                    if(GA==MA.TA)
                        fj("This document requires global security policy to be disabled.\n\nPlease go to Edit > Preferences > JavaScript and uncheck the \"Enable global object security policy\" checkbox. NOTE: In some versions of Adobe Reader you may need to enable JavaScript first.\n\nMessage code: 005");
                    
                    if(GA==MA.ZA)
                        newGuid=rA(R.BA);
                }
                else
                {
                    fj("There is a problem with this protectedpdf document. It may be damaged. Please contact the publisher for a clean file.\n\nMessage code: 060");
                    return "error: 3";
                }
            }
        }
        
        
        if(!EA)
            wA();
        
        if(newGuid==undefined)
        {
            if(oldGuid!=undefined)
            {
                newGuid=NA(oldGuid);
                
                DA(R.BA,new String(newGuid),true);
            }
            else
                newGuid=WA();
        }
        
        gi=newGuid;
        return gi;
    }
    catch(cr)
    {
        return "error: 4";
        
    }
    finally
    {
        this.dirty=false;
    }
}

function NA(oldGuid)
{
    return oldGuid.replace(/^VITR-/g,'');
}

function iA()
{
    return R.fA==""
    ?R.b
    :R.hA+R.fA;
}

function mA(uA)
{
    return Q.test(uA)&&app.viewerVersion>=8;
}

function wA()
{
    oA(R.BA);
    oA(R.b);
}

function sA(xA)
{
    var complete=new Date();
    complete.setTime(complete.getTime()+xA);
    while(new Date().getTime()<complete.getTime());
}

function kA(message)
{
    console.println("\n"+message);
}



var bl=1;
var bw="=";
var p=8;





function ch(ca){return au(o(vitrv(ca),ca.length*p));}
function cg(ca){return at(o(vitrv(ca),ca.length*p));}
function cf(ca){return av(o(vitrv(ca),ca.length*p));}
function bu(al,data){return au(t(al,data));}
function bt(al,data){return at(t(al,data));}
function bs(al,data){return av(t(al,data));}




function o(x,bf)
{
    
    x[bf>>5]|=0x80<<(24-bf%32);
    x[((bf+64>>9)<<4)+15]=bf;
    
    var cn=Array(80);
    var bc=1732584193;
    var bh=-271733879;
    var bk=-1732584194;
    var bj=271733878;
    var cr=-1009589776;
    
    for(var w=0;w<x.length;w+=16)
    {
        var cm=bc;
        var cl=bh;
        var ck=bk;
        var cj=bj;
        var ci=cr;
        
        for(var ba=0;ba<80;ba++)
        {
            if(ba<16)cn[ba]=x[w+ba];
            else cn[ba]=by(cn[ba-3]^cn[ba-8]^cn[ba-14]^cn[ba-16],1);
            var bz=k(k(by(bc,5),br(ba,bh,bk,bj)),
                     k(k(cr,cn[ba]),bq(ba)));
            cr=bj;
            bj=bk;
            bk=by(bh,30);
            bh=bc;
            bc=bz;
        }
        
        bc=k(bc,cm);
        bh=k(bh,cl);
        bk=k(bk,ck);
        bj=k(bj,cj);
        cr=k(cr,ci);
    }
    return Array(bc,bh,bk,bj,cr);
    
}





function br(bz,bh,bk,bj)
{
    if(bz<20)return(bh&bk)|((~bh)&bj);
    if(bz<40)return bh^bk^bj;
    if(bz<60)return(bh&bk)|(bh&bj)|(bk&bj);
    return bh^bk^bj;
}




function bq(bz)
{
    return(bz<20)?1518500249:(bz<40)?1859775393:
    (bz<60)?-1894007588:-899497514;
}




function t(al,data)
{
    var ap=vitrv(al);
    if(ap.length>16)ap=o(ap,al.length*p);
    
    var bv=Array(16),opad=Array(16);
    for(var w=0;w<16;w++)
    {
        bv[w]=ap[w]^0x36363636;
        opad[w]=ap[w]^0x5C5C5C5C;
    }
    
    var hash=o(bv.concat(vitrv(data)),512+data.length*p);
    return o(opad.concat(hash),512+160);
}





function k(x,y)
{
    var cb=(x&0xFFFF)+(y&0xFFFF);
    var ct=(x>>16)+(y>>16)+(cb>>16);
    return(ct<<16)|(cb&0xFFFF);
}




function by(cd,cc)
{
    return(cd<<cc)|(cd>>>(32-cc));
}





function vitrv(aa)
{
    var bb=Array();
    var bd=(1<<p)-1;
    for(var w=0;w<aa.length*p;w+=p)
        bb[w>>5]|=(aa.charCodeAt(w/p)&bd)<<(24-w%32);
    return bb;
}




function av(bb)
{
    var aa="";
    var bd=(1<<p)-1;
    for(var w=0;w<bb.length*32;w+=p)
        aa+=String.fromCharCode((bb[w>>5]>>>(24-w%32))&bd);
    return aa;
}




function au(j)
{
    var aw=bl?"0123456789ABCDEF":"0123456789abcdef";
    var aa="";
    for(var w=0;w<j.length*4;w++)
    {
        aa+=aw.charAt((j[w>>2]>>((3-w%4)*8+4))&0xF)+
        aw.charAt((j[w>>2]>>((3-w%4)*8))&0xF);
    }
    return aa;
}




function at(j)
{
    var cs="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    var aa="";
    for(var w=0;w<j.length*4;w+=3)
    {
        var bi=(((j[w>>2]>>8*(3-w%4))&0xFF)<<16)
        |(((j[w+1>>2]>>8*(3-(w+1)%4))&0xFF)<<8)
        |((j[w+2>>2]>>8*(3-(w+2)%4))&0xFF);
        for(var ba=0;ba<4;ba++)
        {
            if(w*8+ba*6>j.length*32)aa+=bw;
            else aa+=cs.charAt((bi>>6*(3-ba))&0x3F);
        }
    }
    return aa;
}


function gt(input){
    var keyStr="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    var output="";
    var chr1,chr2,chr3;
    var enc1,enc2,enc3,enc4;
    var w=0;
    
    do{
        chr1=input.charCodeAt(w++);
        chr2=input.charCodeAt(w++);
        chr3=input.charCodeAt(w++);
        
        enc1=chr1>>2;
        enc2=((chr1&3)<<4)|(chr2>>4);
        enc3=((chr2&15)<<2)|(chr3>>6);
        enc4=chr3&63;
        
        if(isNaN(chr2)){
            enc3=enc4=64;
        }else if(isNaN(chr3)){
            enc4=64;
        }
        
        output=output+keyStr.charAt(enc1)+keyStr.charAt(enc2)+
        keyStr.charAt(enc3)+keyStr.charAt(enc4);
    }while(w<input.length);
    
    return output;
}



var fm=false;
var dp="ppdf";
var dv=false;
var fs=new Array();
fs[0]='0';
fs[1]='1';
var fr=new Array();
fr[0]='W';
fr[1]='X';

function be()
{
    if(dv)
        return;
    
    am();
    
    if(FA)
        return;
    
    try
    {
        fm=false;
        if(KA())
        {
            fm=true;
            ag();
            
            
            if(U()!="")
            {
                if(dx(p_alertofflinedays))
                    fk("You have @offlineDays@ day(s) of offline access left.\n\nMessage code: 008");
            }
        }
        
        if(dx(p_pingonstartup))
            bx(true);
        
        
        
        if(!df&&(global.boom_sso!=null)&&(global.boom_sso[q]!=null))
        {
            
            var d=this.getField("boom.form.response.username");
            var h=this.getField("boom.form.neversend.pwd");
            
            if(d!=undefined)
                d.value=global.boom_sso[q].username;
            if(h!=undefined)
                h.value=global.boom_sso[q].gr;
            
            dn=true;
            cy();
        }
    }
    finally
    {
        this.dirty=false;
        dv=true;
    }
}


function aq(bn)
{
    var d=this.getField("boom.form.response.username");
    var h=this.getField("boom.form.neversend.pwd");
    var eu=this.getField("boom.form.response.docid");
    var e=this.getField("boom.version");
    
    var email=d==undefined?"":IA(d.value.toString());
    var gr=h==undefined?"":IA(h.value.toString());
    var docid=eu==undefined?"":eu.value;
    var version=e==undefined?"":e.value.toString();
    
    var l=new Date();
    var bp=1000*60*60*24;
    l=new Date(l.getTime()-(bp*bn));
    var bo=email+gr+docid+version+l.getUTCMonth()+l.getUTCDate()+l.getUTCFullYear();
    
    var hash=ch(bo);
    return hash.substring(0,10);
}

function n()
{
    try
    {
        u();
        var h=this.getField("boom.form.neversend.pwd");
        
        if(h!=null)
        {
            if(!dx(p_showpassword))
            {
                var de=this.getField("boom.form.pwd_label");
                de.display=display.hidden;
                h.display=display.hidden;
                h.value="ignore";
            }
            else
            {
                h.value="";
            }
        }
    }
    finally
    {
        this.dirty=false;
    }
}

function ag()
{
    try
    {
        r();
        
        var d=this.getField("boom.form.response.username");
        var h=this.getField("boom.form.neversend.pwd");
        
        
        
        
        var email=d==undefined?"":IA(d.value.toString());
        var gr=h==undefined?"":IA(h.value.toString());
        
        if(!fm)
        {
            
            if(global.boom_sso==null)
            {
                global.boom_sso=new Object();
            }
            if(global.boom_sso[q]==null)
            {
                global.boom_sso[q]=new Object();
            }
            
            global.boom_sso[q].username=email;
            if(gr!="")
                global.boom_sso[q].gr=gr;
        }
        
        
        if(email!=""){
            ds=new String(email);
            d.value="";
        }
        
        if(h!=undefined)
            h.value="";
        
        if(ec)
            ee();
    }
    finally
    {
        this.dirty=false;
    }
}


function ai()
{
    var c=this.getField("boom.unlocked.rememberme");
    
    if(c.isBoxChecked(0))
    {
        CA();
    }
    else{
        dh(true);
    }
    this.dirty=false;
}

function da()
{
    n();
}

function db()
{
    n();
}










function cy()
{
    var x;
    if((typeof ppdfValidate=="function")&&!ppdfValidate())
        return;
    
    if(dx(p_usechallengeresponse))
        x = fp();
    else 
        x = fq();
    return x;
}

function fp()
{
    var x;
    try
    {
        
        var m=["boom.form.response","boom.version","boom.hidden.gen"];
        var ay=U(true);
        
        if(ay!="")
            x = C(A.JA,m);
        
    }
    catch(cr)
    {
        
        if(xy(cr.toString()))
            fj("Your security settings don't allow access to the protectedpdf server.  You must allow access to the server in order to open the document.\n\nMessage code: 055");
        else 
            fj("There is a problem with this protectedpdf document. It may be damaged. Please contact the publisher for a clean file.\n\nMessage code: 061");
    }
    
    return x;
}




function fq()
{
    var h=this.getField("boom.form.neversend.pwd");
    if(h!=null)
    {
        try
        {
            
            var m=["boom.form.response","boom.version","boom.hidden.gen"];
            var ay=U(true);
            
            if(ay!="")
                C(A.HA,m);
        }
        catch(cr)
        {
            
            if(xy(cr.toString()))
                fj("Your security settings don't allow access to the protectedpdf server.  You must allow access to the server in order to open the document.\n\nMessage code: 070");
            else 
                fj("There is a problem with this protectedpdf document. It may be damaged. Please contact the publisher for a clean file.");
        }
    }
}

function a(bm,VA)
{
    var d=this.getField("boom.form.response.username");
    var h=this.getField("boom.form.neversend.pwd");
    
    try{
        var gr=h==null?"":IA(h.value.toString());
        var username=d==null?"":IA(d.value.toString());
        
        
        if(!VA)
            gr=h==null?"":gr.toLowerCase();
        
        var hash=h==null?"":bu(gr+username,bm);
        
        
        var m=["boom.form.response","boom.version","boom.hidden.gen"];
        C(A.a,m,false,hash);
        this.dirty=false;
    }
    catch(cr){
        
        if(xy(cr.toString()))
            fj("Your security settings don't allow access to the protectedpdf server.  You must allow access to the server in order to open the document.\n\nMessage code: 071");
        else 
            fj("There is a problem with this protectedpdf document. It may be damaged. Please contact the publisher for a clean file.\n\nMessage code: 066");
    }
}

function dz()
{
    try
    {
        if(!df)
        {
            fj("You cannot perform this action unless the document is unlocked.\n\nMessage code: 009");
            return;
        }
        
        
        var dr=ds;
        
        if(dr==null)
        {
            if(T(R.S+"_J")!=null)
                dr=T(R.S+"_J");
            else if(X!=undefined&&X.username!=null)
                dr=X.username;
        }
        
        var d=this.getField("boom.form.response.username");
        
        if(d!=null&&d.value=="")
            d.value=dr;
        
        
        var m=["boom.form.response","boom.version","boom.hidden.gen"];
        var ay=U(true);
        
        if(ay!="")
            C(A.LA,m);
    }
    catch(cr)
    {
        
        if(xy(cr.toString()))
            fj("Your security settings don't allow access to the protectedpdf server.  You must allow access to the server in order to uninstall the license.\n\nMessage code: 056");
        else 
            fj("There is a problem with this protectedpdf document. It may be damaged. Please contact the publisher for a clean file.\n\nMessage code: 063");
    }
    finally
    {
        this.dirty=false;
    }
}

function dy(bm)
{
    try
    {
        var d=this.getField("boom.form.response.username");
        var email=d==undefined?"":IA(d.value.toString());
        
        var hash=bu(email,bm);
        
        
        var m=["boom.form.response","boom.version","boom.hidden.gen"];
        C(A.dy,m,false,hash);
        
        if(d!=null)
            d.value="";
    }
    catch(cr)
    {
        
        if(xy(cr.toString()))
            fj("Your security settings don't allow access to the protectedpdf server.  You must allow access to the server in order to uninstall the license.\n\nMessage code: 056");
        else 
            fj("There is a problem with this protectedpdf document. It may be damaged. Please contact the publisher for a clean file.\n\nMessage code: 067");
    }
    finally
    {
        this.dirty=false;
    }
}

function ee()
{
    if(dj!=null)
    {
        var ed=this.getField("boom.water");
        ed.value=dj;
    }
}


function en()
{
    try
    {
        var er=this.getField("boom.form.offline.uc");
        var usernameField=this.getField("boom.form.response.username");
        var unlockCode=er==undefined?"":IA(er.value.toString());
        var username=usernameField==undefined?"":IA(usernameField.value.toString());
        
        if((unlockCode=="")||(username==""))
        {
            fj("Please provide your unlock code and credential.\n\nMessage code: 010");
        }
        else{
            var originalUnlockCode=fn(unlockCode);
            var ez=originalUnlockCode.substring(originalUnlockCode.length-8,originalUnlockCode.length-1);
            var fe=originalUnlockCode.substring(originalUnlockCode.length-1,originalUnlockCode.length);
            var SA=this.getField("boom.form.offline.ac");
            var vA=SA==undefined?"":SA.value.toString();
            var eu=this.getField("boom.form.response.docid");
            
            var fg=em(fn(vA),eu.value.toString(),username,ez,originalUnlockCode.length);
            if((fg.toLowerCase()+ei(fg)==originalUnlockCode.toLowerCase())&&(fe==ei(fg)))
            {
                var fa=parseInt(ez,16);
                var fh=(fa.toString()).substring(0,4);
                var fb=(fa.toString()).substring(4,6);
                var fc=(fa.toString()).substring(6,8);
                
                
                if(fb[0]=="0")
                    fb=fb[1];
                if(fc[0]=="0")
                    fc=fc[1];
                
                
                var fd=new Date(parseInt(fh),parseInt(fb)-1,parseInt(fc),23,59,59);
                
                var l=new Date();
                var vitrV=new Date(l.getUTCFullYear(),l.getUTCMonth(),l.getUTCDate(),l.getUTCHours(),l.getUTCMinutes(),l.getUTCSeconds());
                
                if(fd<vitrV)
                {
                    fj("Your offline access has expired. You need to re-login.\n\nMessage code: 007");
                }
                else
                {
                    bg=Math.floor((fd.getTime()-vitrV.getTime())/ao);
                    fv=true;
                    cq=true;
                    r();
                }
            }
            else
            {
                fj("This unlock code is invalid.  Please make sure the credential and unlock code are entered correctly.\n\nMessage code: 011");
            }
        }
    }
    catch(cr)
    {
        fj("This unlock code is invalid.  Please make sure the credential and unlock code are entered correctly.\n\nMessage code: 011");
    }
}

function em(ac,docID,XA,ez,ff)
{
    var ex=(au(t(dw,ac.toLowerCase()+docID.toLowerCase()+XA.toLowerCase())).substring(0,ff-8))+ez;
    
    return ex;
}

function ei(fg)
{
    var fi=0;
    for(var w=0;w<fg.length;w++)
    {
        fi+=parseInt(fg[w],16);
    }
    
    return fi%10;
}

function ej()
{
    try
    {
        var isNoInternet=this.getField("boom.form.offline_ckb");
        if(isNoInternet.isBoxChecked(0))
        {
            ef();
            el();
            if(dx(p_alertonphonetemplate))
                fk(p_phoneunlockinstructions);
        }
        else
        {
            eg();
        }
    }
    finally
    {
        this.dirty=false;
    }
}

function el()
{
    var RA=this.getField("boom.form.offline.ac");
    var ey=au(t(dw,U(true)));
    
    RA.value=fo(ey.substring(0,ew));
}

function fo(code)
{
    return(code.replace(new RegExp(fs[0],"g"),fr[0])).replace(new RegExp(fs[1],"g"),fr[1]);
}

function fn(code)
{
    return(code.replace(new RegExp(fr[0],"gi"),fs[0])).replace(new RegExp(fr[1],"gi"),fs[1]);
}

if(!(app.viewerVersion<7))
{
    
    app.setTimeOut("be()",200);
}

function IA(aa){
    return aa.replace(/^\s+|\s+$/g,'');
    
}