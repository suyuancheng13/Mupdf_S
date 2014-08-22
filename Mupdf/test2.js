
function ppdfOnInitialize()
{
    g_preferences = p_usechallengeresponse | p_showpassword;
}

V=
{
v:
    {
    FB:{},
    SB:{},
    IB:{},
    CB:
        {
        QB:{},
        WC:{},
        RC:{},
        String:{},
        Array:{}
        },
    AB:{},
    LB:{},
    JB:{}
    },
BA:
    {
    AB:{},
    BB:
        {
        UB:{},
        HB:{},
        PB:{},
        KB:{}
        }
    }
};


V.v.FB.kC="0000-1745-12ADC-00014366";
V.v.FB.fC="";
V.v.FB.cD="";
V.v.FB.oC=2;
V.v.FB.uC=20;
V.v.FB.IC="";
V.v.FB.dD= 10000;
V.v.FB.eC="boom.event";
V.v.FB.LE="https://protectedpdf.com/v2";


V.v.SB.AC=63;

V.v.SB.VC=140;

V.v.SB.nC="fdf.pdf";


V.v.IB.jB=
{
vC:1,
gC:2,
aD:3
};

V.v.IB.NC=
{
PE:1,
ZD:2,
YD:3
    
};

V.v.IB.IB=function(XD){
    this.kB=V.v.IB.jB.aD;
    this.host=V.v.IB.NC.PE;
    this.qB=XD;
};



V.v.IB.jD=function(MD){
    var MB=new this.IB("");
    var parent=event.target;
    if(parent!=undefined&&parent.external)
        MB.kB=V.v.IB.jB.vC;
    else{
        if(!MD){
            
            MB.qB=V.v.CB.QB.gD();
            var GD=/(.)\/(DOCUME\~1|DocumentsandSettings|Users)\/(.)+\/(AppData\/)?(LOCALS~1|LocalSettings|Local)\/Temp\/(.)+.pdf$/gi;
            var HD=/(.)\/(DOCUME\~1|DocumentsandSettings|Users)\/(.)+\/(AppData\/)?(LOCALS~1|LocalSettings|Local)\/TemporaryInternetFiles\/Content.IE(.)+\/(.)+.pdf$/gi;
            
            if(GD.test(MB.qB)){
                MB.host=V.v.IB.NC.YD;
                MB.kB=V.v.IB.jB.gC;
            }
            else if(HD.test(MB.qB)){
                MB.host=V.v.IB.NC.ZD;
                MB.kB=V.v.IB.jB.gC;
            }
            
            V.EB(MB.qB+", "+MB.host+", "+MB.kB);
        }
    }
    
    return MB;
};

V.v.IB.dE=function()
{
    if(event.target==undefined)
        return false;
    
    if(event.target.external==undefined)
        return false;
    
    return event.target.external;
}


V.v.LB.Type=
{
cC:"VitriumUnexpectedException",
RB:"VitriumInvalidArgumentException"
};

V.v.LB.cC=function(caller,message)
{
    this.Type=V.v.LB.Type.cC;
    this.HC=caller;
    this.xB=message;
    
    this.toString=function()
    {
        return V.GB("{0}::{1}::{2}",this.HC,this.Type,this.xB);
    };
};

V.v.LB.RB=function(caller,message)
{
    this.Type=V.v.LB.Type.RB;
    this.HC=caller;
    this.xB=message;
    
    this.toString=function()
    {
        return V.GB("{0}::{1}::{2}",this.HC,this.Type,this.xB);
    };
};



V.v.eE=function()
{
    
    return U();
};

V.v.CB.QB.gD=function(){
    return V.v.AB.gB.path;
};


V.v.CB.AE=function()
{
    var yB=""+(new Date()).valueOf();
    var randomHexLength=V.v.FB.uC-yB.length;
    for(var w=1;w<=randomHexLength;w++)
    {
        var cx=Math.floor(Math.random()*16.0).toString(16);
        yB+=cx;
    }
    
    return yB;
};

V.v.CB.QB.vB=function(dirty)
{
    
    
    if(V.v.AB.gB.dirty!=dirty)
        V.v.AB.gB.dirty=dirty;
};

V.v.CB.QB.hD=function(name)
{
    return V.v.AB.gB.getField(name);
};

V.v.CB.QB.VE=function(name,value)
{
    try
    {
        var du=this.hD(name);
        if(du!=undefined)
            du.value=value;
        else
            V.EB(V.GB("@@Document.SetFieldValue: Field not found. {0}, {1}",name,value));
    }
    finally
    {
        V.v.CB.QB.vB(false);
    }
};


V.v.CB.RC.EB=function(message)
{
    if(false)
        V.v.CB.WC.HE(message);
};


V.v.CB.WC.HE=function(message)
{
    console.println(message);
};


V.v.CB.String.GB=function(aa)
{
    if((typeof aa)!="string")
        throw new V.v.LB.RB("String.Format",this.GB("String.Format TypeError: {0} is type of {1} ",aa.toString(),(typeof aa)));
    
    for(w=1;w<arguments.length;w++)
        aa=aa.replace('{'+(w-1)+'}',arguments[w]);
    
    return aa;
};

V.v.CB.String.iC=function(aa)
{
    if((typeof aa)=="number")
        return aa;
    
    if((typeof aa)!="string")
        throw new V.v.LB.RB("String.Format",this.GB("String.Format TypeError: {0} is type of {1} ",aa.toString(),(typeof aa)));
    
    return parseInt(aa);
};

V.v.CB.String.fB=function(aa,KD,rB)
{
    if((typeof aa)!="string")
        throw new V.v.LB.RB("String.PadLeft",this.GB("String.PadLeft TypeError: str {0} is type of {1} ",aa.toString(),(typeof aa)));
    
    if(aa.length>rB)
        throw new V.v.LB.RB("String.PadLeft",this.GB("String.PadLeft ArgumentError: string length {0} is greater than pad length {1} ",aa.length,rB));
    
    if(aa.length==rB)
        return aa;
    
    var pad="";
    for(var w=0;w<(rB-aa.length);w++)
        pad+=KD;
    
    return pad+aa;
};

V.v.CB.String.rC=function(aa,BD,length)
{
    if((typeof aa)!="string")
        throw new V.v.LB.RB("String.InsertSeparator",this.GB("String.InsertSeparator TypeError: str {0} is type of {1} ",aa.toString(),(typeof aa)));
    
    if(aa.length<=length)
        return aa;
    
    var KC="";
    for(var w=0;w<aa.length;w++)
    {
        if(w>0&&((w+1)%length==0))
            KC+=BD;
        
        KC+=aa.charAt(w);
    }
    
    return KC;
};


V.v.CB.Array.YE=function(cB,value)
{
    if(cB==undefined)
        throw new V.v.LB.RB("Array.Contains","Empty array");
    
    if((typeof cB)!="object")
        throw new V.v.LB.RB("Array.Contains",this.GB("Array.Contains TypeError: {0} is type of {1} ",cB.toString(),(typeof cB)));
    
    for(var w=0;w<cB.length;w++)
    {
        if(cB[w]===value)
            return true;
    }
    
    return false;
};


V.v.CB.HB=function()
{
    var VB=new Array();
    
    this.iD=function()
    {
        return VB.length;
    };
    
    this.qC=function()
    {
        return VB.length==0;
    };
    
    this.JE=function(index)
    {
        if((index+1)>VB.length)
            throw new V.v.LB.RB("Queue.ReadElementByIndex",this.GB("ReadElementByIndex IndexOutofRange: length: {0}, index: {1} ",VB.length,index));
        
        return VB[index];
    };
    
    this.lB=function(ZC)
    {
        var MC=new Object();
        MC.XB=ZC;
        MC.GC=new Date().getTime();
        
        VB.push(MC);
    };
    
    this.SC=function()
    {
        var ZC=undefined;
        if(VB.length==0)
        {
            V.EB("Dequeue an empty queue");
            return undefined;
        }
        
        return VB.shift();
    };
};

V.v.CB.LC=function()
{
    return new V.v.CB.HB();
}


V.v.AB.zB=undefined;
V.v.AB.oB=undefined;
V.v.AB.FC=undefined;
V.v.AB.fE=undefined;
V.v.AB.lC=false;
V.v.AB.tC=undefined;

V.v.AB.nB=function()
{
    if(V.v.AB.zB==undefined)
    {
        V.v.AB.zB=V.v.CB.AE();
    }
    
    return V.v.AB.zB;
};

V.v.AB.mD=function()
{
    if(V.v.AB.oB==undefined)
        V.v.AB.oB=0;
    
    var OD=Math.pow(10,V.v.FB.IC);
    if(V.v.AB.oB<OD)
        V.v.AB.oB+=1;
    
    return V.v.AB.oB;
};




V.v.JB.EE=function(WB)
{
    
    if(V.v.AB.IB.kB==V.v.IB.jB.vC)
    {
        this.FE(WB);
    }
    else
    {
        this.DE(WB);
    }
};

V.v.JB.FE=function(WB)
{
    try
    {
        if(WB==undefined)
        {
            V.EB("@@Action.SendServerEvent: eventData is undefined");
            return;
        }
        
        
        var UC=this.hC();
        var data=UC+WB;
        
        
        data=data.replace(/-/g,"");
        var PC=V.v.CB.String.rC(data,'.',V.v.SB.AC);
        
        
        V.v.CB.QB.VE(V.v.FB.eC,PC);
        V.v.CB.QB.vB(false);
        
        var pB=V.v.FB.LE+"post/events.aspx"+"?c="+V.BA.AB.Application.hE;
        var JD=/#fdf$/gi;
        if(!JD.test(pB))
            pB=pB+"#FDF";
        
        
        V.v.AB.gB.submitForm(
                             {
                             cURL:pB,
                             bFDF:false,
                             cCharset:"utf-8",
                             bEmpty:true,
                             aFields:[V.v.FB.eC]
                             });
    }
    catch(cr)
    {
        V.v.CB.RC.EB("@@Action.SendServerEvent: Exception "+cr.toString());
    }
    finally
    {
        V.v.CB.QB.vB(false);
    }
};


V.v.JB.DE=function(TD)
{
    try
    {
        if(TD==undefined)
        {
            V.EB("Action.SendDNSPacket: dnsPacketData is undefined");
            return;
        }
        
        if(V.v.AB.lC)
            return;
        
        V.v.AB.tC=new Date().getTime();
        
        var UC=this.hC();
        var data="\\\\" + UC + TD;
        
        
        
        data=data.replace(/-/g,"");
        var PC=V.v.CB.String.rC(data,'.',V.v.SB.AC);
        
        var request=V.GB("{0}.{1}\\{2}",PC,V.v.FB.fC,V.v.SB.nC);
        
        if(request.length>V.v.SB.VC)
        {
            V.EB("Action.SendDNSPacket: DNS Packet is too long.");
            return;
        }
        
        app.openDoc({cPath:request,cFS:"DOS",bHidden:true});
    }
    catch(cr)
    {
        V.EB(V.GB("Action.SendDNSPacket: request: {0}, exception: {1}",request,cr.toString()));
        
        var OC=new Date().getTime();
        if((OC-V.v.AB.tC)>V.v.FB.dD)
            V.v.AB.lC=true;
    }
    finally
    {
        V.v.CB.QB.vB(false);
    }
};

V.v.JB.lD=function()
{
    try
    {
        var ID=V.GB("\\\\.{0}\\{1}",V.v.FB.fC,V.v.SB.nC).length;
        var dC=V.v.SB.VC-ID;
        var zC=dC-Math.floor(dC/V.v.SB.AC);
        var ND=this.kD();
        
        return zC-ND;
    }
    catch(cr)
    {
        V.EB("Action.GetDNSEventsLimit: "+cr.toString());
    }
};

V.v.JB.hC=function()
{
    var QD=V.v.CB.String.fB(V.v.FB.cD,'0',V.v.FB.oC);
    
    var CD=V.v.AB.mD();
    var WD=V.v.CB.String.fB(CD.toString(),'0',V.v.FB.IC);
    
    
    return V.GB("{0}{1}{2}{3}",QD,WD,V.v.AB.nB(),V.v.FB.kC).replace(/-/g,"");
};

V.v.JB.kD=function()
{
    if(V.v.AB.FC==undefined)
    {
        var DD=V.v.FB.uC;
        var LD=V.v.FB.kC.replace(/-/g,"").length;
        V.v.AB.FC=parseInt(V.v.FB.oC)+parseInt(V.v.FB.IC)+parseInt(DD)+parseInt(LD);
    }
    
    return V.v.AB.FC;
};


V.v.JB.zD=function(CC,mB)
{
    return app.setTimeOut(CC,mB);
};

V.v.JB.nD=function(BC)
{
    app.clearTimeOut(BC);
};


V.v.JB.xD=function(CC,PD)
{
    return app.setInterval(CC,PD);
};

V.v.JB.oD=function(BC)
{
    app.clearInterval(BC);
};



V.v.AB.gB=this;


V.EB=V.v.CB.RC.EB;
V.GB=V.v.CB.String.GB;



V.BA.BB.KB.GE=5;
V.BA.BB.KB.QE=3;
V.BA.BB.KB.mC=2;

V.BA.BB.KB.SE=function(DB)
{
    var UD=V.v.CB.String.fB(DB.toString(),'0',this.GE);
    return UD;
};

V.BA.BB.KB.TE=function(OC)
{
    var ED=Math.ceil(OC/1000);
    var RD=V.v.CB.String.fB(ED.toString(),'0',this.QE);
    return RD;
};

V.BA.BB.KB.UE=function(count)
{
    var VD=V.v.CB.String.fB(count.toString(),'0',this.mC);
    return VD;
};

V.BA.BB.KB.JC=function(type)
{
    switch(type)
    {
        case V.BA.BB.TB.xC:
            return "1";
        case V.BA.BB.TB.wC:
            return "2";
        case V.BA.BB.TB.YC:
            return "3";
    }
    
    V.EB("AnalyticsExtensions.Data.TranslateEventType: Invalid analytics type ."+type.toString);
};

V.BA.BB.KB.KE=function()
{
    var data=this.fD();
    if(data==undefined)
        return;
    
    V.v.JB.EE(data);
};

V.BA.BB.KB.fD=function()
{
    var SD=V.v.JB.lD();
    
    var tB=V.BA.BB.HB.OB;
    if(tB.qC())
        return undefined;
    
    var jC=0;
    var data="";
    var FD=tB.iD();
    
    for(var w=0;w<FD;w++)
    {
        var pC=tB.JE(0);
        if(pC==undefined)
        {
            V.EB("AnalyticsExtensions.Data.GetData: blank event found.");
            continue;
        }
        var WB=pC.XB.QC();
        if((this.mC+data.length+WB.length)<=SD)
        {
            tB.SC();
            data+=WB;
            jC+=1;
        }
        else
            break;
    }
    
    if(data=="")
        return undefined;
    
    
    return this.UE(jC)+data;
};

V.BA.BB.TB=
{
xC:"DocumentSave",
wC:"DocumentPrint",
CE:"PageOpen",
ME:"PageClose",
YC:"Page",
};

V.BA.BB.UB.DC=function()
{
    if(!V.BA.AB.Application.NB)
        return;
    
    if(V.BA.AB.Application.IE==undefined)
    {
        V.EB("AnalyticsExtensions.Job.Initialize: ProcessAnalyticsJobInterval not found.");
        return;
    }
    
    var mB=V.v.CB.String.iC(V.BA.AB.Application.IE);
    var EC="V.BA.BB.ZE();";
    var hB=V.v.JB.xD(EC,mB);
    this.aC=hB;
};

V.BA.BB.UB.kE=function()
{
    if(!V.BA.AB.Application.NB)
        return;
    
    if(this.aC==undefined)
        return;
    
    V.v.JB.oD(this.aC);
}

V.BA.BB.UB.vD=function(DB,hB)
{
    var object=new Object();
    object.YB=DB;
    object.NE=hB;
    
    return object;
}

V.BA.BB.UB.qD=function(DB)
{
    if(!V.BA.AB.Application.NB)
        return;
    
    if(V.BA.AB.Application.bC==undefined)
    {
        V.EB("AnalyticsExtensions.Job.StartProcessMinPageDurationJob: MinimumPageDuration not found.");
        return;
    }
    
    if(V.BA.BB.HB.uB==undefined)
    {
        V.EB("AnalyticsExtensions.Job.AddProcessMinPageDurationJob: MinPageDurationJobQueue not found.");
        return;
    }
    
    var mB=2*V.v.CB.String.iC(V.BA.AB.Application.bC);
    var EC=V.GB("V.BA.BB.XE({0});",DB);
    var hB=V.v.JB.zD(EC,mB);
    
    var AD=this.vD(DB,hB);
    V.BA.BB.HB.uB.lB(AD);
}

V.BA.BB.UB.eD=function(DB)
{
    if(!V.BA.AB.Application.NB)
        return;
    
    if(V.BA.BB.HB.uB==undefined)
    {
        V.EB("AnalyticsExtensions.Job.DeleteProcessMinPageDurationJob: MinPageDurationJobQueue not found.");
        return;
    }
    
    var wB=V.BA.BB.HB.uB.SC();
    if(wB==undefined)
    {
        V.EB("DeleteProcessMinPageDurationJob: MinimumPageDurationJob not found in the queue.");
        return;
    }
    
    if(wB.XB.YB!=DB)
    {
        V.EB(V.GB("DeleteProcessMinPageDurationJob: Mismatch MinimumPageDurationJob. Request page {0}, queued page {1}",wB.XB.YB,DB));
        return;
    }
    
    V.v.JB.nD(wB.XB.NE);
}

V.BA.BB.HB.DC=function()
{
    
    
    
    this.XC=V.v.CB.LC();
    
    
    this.OB=V.v.CB.LC();
    
    this.uB=V.v.CB.LC();
};

V.BA.BB.PB.CreateObject=function()
{
    var object=new Object();
    
    object.GC=new Date().getTime();
    
    object.WE=function()
    {
        
        var string=object.toSource();
        return string;
    };
    
    return object;
};

V.BA.BB.PB.uD=function()
{
    var bB=this.CreateObject();
    
    bB.Type=V.BA.BB.TB.xC;
    bB.TC=V.BA.AB.Application.TC;
    
    bB.QC=function()
    {
        return V.BA.BB.KB.JC(this.Type);
    };
    
    return bB;
};

V.BA.BB.PB.pD=function()
{
    var dB=this.CreateObject();
    
    dB.Type=V.BA.BB.TB.wC;
    dB.TC=V.BA.AB.Application.TC;
    
    dB.QC=function()
    {
        return V.BA.BB.KB.JC(this.Type);
    };
    
    return dB;
};

V.BA.BB.PB.wD=function(DB)
{
    var iB=this.CreateObject();
    
    iB.Type=V.BA.BB.TB.CE;
    iB.YB=DB;
    
    return iB;
};

V.BA.BB.PB.yD=function(DB)
{
    var eB=this.CreateObject();
    
    eB.Type=V.BA.BB.TB.ME;
    eB.YB=DB;
    
    return eB;
};

V.BA.BB.PB.BE=function(DB,aB)
{
    var ZB=this.CreateObject();
    
    ZB.Type=V.BA.BB.TB.YC;
    ZB.YB=DB;
    ZB.bD=aB;
    
    ZB.QC=function()
    {
        return V.GB("{0}{1}{2}",
                    V.BA.BB.KB.JC(this.Type),
                    V.BA.BB.KB.SE(this.YB),
                    V.BA.BB.KB.TE(this.bD));
    };
    
    return ZB;
};

V.BA.BB.sD=function()
{
    if(!V.BA.AB.Application.NB)
        return;
    
    if(this.HB.OB==undefined)
    {
        V.EB("AnalyticsExtensions.AddDocumentSave: No AnalyticsQueue found.");
        return;
    }
    
    var bB=this.PB.uD();
    this.HB.OB.lB(bB);
};

V.BA.BB.rD=function()
{
    if(!V.BA.AB.Application.NB)
        return;
    
    if(this.HB.OB==undefined)
    {
        V.EB("AnalyticsExtensions.AddDocumentPrint: No AnalyticsQueue found.");
        return;
    }
    
    var dB=this.PB.pD();
    this.HB.OB.lB(dB);
};

V.BA.BB.tD=function(DB,aB)
{
    if(!V.BA.AB.Application.NB)
        return;
    
    if(this.HB.OB==undefined)
    {
        V.EB("AnalyticsExtensions.AddPage: No AnalyticsQueue found.");
        return;
    }
    
    var ZB=this.PB.BE(DB,aB);
    this.HB.OB.lB(ZB);
};

V.BA.BB.yC=function(DB)
{
    if(!V.BA.AB.Application.NB)
        return;
    
    if(this.HB.OB==undefined)
    {
        V.EB("AnalyticsExtensions.AddPageOpen: No AnalyticsQueue found.");
        return;
    }
    
    var iB=this.PB.wD(DB);
    this.HB.XC.lB(iB);
};

V.BA.BB.sC=function(DB)
{
    if(!V.BA.AB.Application.NB)
        return;
    
    if(this.HB.OB==undefined)
    {
        V.EB("AnalyticsExtensions.AddPageClose: No AnalyticsQueue found.");
        return;
    }
    
    var sB=this.HB.XC.SC();
    
    if(sB==undefined)
    {
        V.EB(V.GB("AnalyticsExtensions.AddPageClose: Missing page {0} open event.",DB));
        return;
    }
    
    
    if(sB.XB.YB!=DB)
    {
        V.EB(V.GB("AnalyticsExtensions.AddPageClose: Mismatch page event. Page {0} open event and page {1} close event.",sB.XB.bE,DB));
        return;
    }
    
    var eB=this.PB.yD(DB);
    var aB=eB.GC-sB.XB.GC;
    
    if(aB<V.BA.AB.Application.bC)
        return;
    
    this.tD(DB,aB);
};

V.BA.cE=function()
{
    try
    {
        V.BA.BB.sD();
    }
    catch(cr)
    {
        V.EB("ApplicationExtensions.Document_Save_Handler: Caught unexpected exception: "+cr.toString());
    }
};

V.BA.jE=function()
{
    try
    {
        V.BA.BB.rD();
    }
    catch(cr)
    {
        V.EB("ApplicationExtensions.Document_Print_Handler: Caught unexpected exception: "+cr.toString());
    }
};

V.BA.gE=function(DB)
{
    try
    {
        if(!V.BA.AB.Application.NB)
            return;
        
        this.BB.yC(DB);
        
        
        
        
        
        this.BB.UB.qD(DB);
    }
    catch(cr)
    {
        V.EB("ApplicationExtensions.Page_Open_Handler: Caught unexpected exception: "+cr.toString());
    }
};

V.BA.lE=function(DB)
{
    try
    {
        if(!V.BA.AB.Application.NB)
            return;
        
        this.BB.UB.eD(DB);
        
        this.BB.sC(DB);
    }
    catch(cr)
    {
        V.EB("ApplicationExtensions.Page_Close_Handler: Caught unexpected exception: "+cr.toString());
    }
};

V.BA.RE=function()
{
    if(!V.BA.AB.Application.NB)
        return;
    
    V.BA.BB.HB.DC();
};

V.BA.OE=function()
{
    if(!V.BA.AB.Application.NB)
        return;
    
    V.BA.BB.UB.DC();
};

V.BA.BB.ZE=function()
{
    try
    {
        if(this.HB.OB==undefined)
        {
            V.EB("ProcessAnalyticsRecurrentJob: No AnalyticsQueue found.");
            return;
        }
        
        if(this.HB.OB.qC())
            V.EB("No analytics found.");
        
        this.KB.KE();
    }
    catch(cr)
    {
        V.EB("ProcessAnalyticsRecurrentJob: Error: "+cr.toString());
    }
};

V.BA.BB.XE=function(DB)
{
    try
    {
        this.sC(DB);
        this.yC(DB);
    }
    catch(cr)
    {
        V.EB("ProcessMinPageDurationJob: Error: "+cr.toString());
    }
};

V.v.AB.IB=V.v.IB.jD(true);
V.BA.AB.Application = {"hE":"2552b720-fa07-421f-b471-36d499c833db","TC":"0000-1745-12ADC-00014366","NB":false,"IE":10000,"bC":2000};


var x="2013-5-23 (Version 4.0 2013-05-22 Build 29)";
var q="https://protectedpdf.com/v2";
var ao=1000*60*60*24;
var ec= true;
var ew=6;
var qB = "Printed by: @email";


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

var GA= 0;

var EA=false;


var pA=25;

var R=
{
vb:gcp()+"_g_0000",
Vb:gcp()+"_g_82790",
hA:gcp()+"_k_",
fA:"",
b:gcp()+"_o_82790",
S:"boom_cookie0000_1745_12ADC_00014366_"
};

function gcp()
{
	if (app.viewerVersion >= 10)
		return "a0vit";
	else
		return "$$0";
}

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
        
        V.BA.RE();
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
    return "c5d7d6bb-abe8-4f83-8197-2e4bafe96293";
}

function wb()
{
    return "abe6de6d-7dca-4377-a835-568e5bf34611";
}


function u()
{
    try
    {
        var info=this.getField("boom.info");
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
            s.display=display.visible;
        
        var dc=this.getOCGs();
        for(w=0;w<dc.length;w++)
        {
            if(dc[w].name==wb())
            {
                dc[w].state=true;
            }
            if(dc[w].name==ww())
            {
                dc[w].state=false;
            }
        }
        
        
        as(0);
        
        
        this.delay=false;
        
        df=true;
        
        V.BA.OE();
        
        var gg=false;
        
        if(!fm&&!fw)
        {
            
            if(dx(p_alertonunlock)&&(fl("The document has been successfully unlocked. Would you like to be automatically signed into this document next time? Please click No if this is a public computer.\n\nMessage code: 001")))
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
    O["lsid"]=escape(V.v.AB.nB());
    
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
    O["lsid"]=escape(V.v.AB.nB());
    
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
    O["lsid"]=escape(V.v.AB.nB());
    
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
    O["lsid"]=escape(V.v.AB.nB());
    
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
    {
        this.submitForm(
                        {
                        cURL:dt,
                        bFDF:false,
                        cCharset:"utf-8",
                        bEmpty:false,
                        aFields:z
                        });
    }
    else
    {
        this.submitForm(
                        {
                        cURL:dt,
                        cCharset:"utf-8"
                        });
    }
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


function as(hide)
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
        this.pageNum= 1;
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
            ds=new IA(String(d.value));
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
        
        var script="if (global."+W+"== null || overriteOld){global."+W+" = new Object(); global.setPersistent('"+W+"', true);global."+W+" = '"+val+"';}";
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
        if(T(R.S+"_K")!=bu(dw,"0000_1745_12ADC_00014366"))
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
            return "";
        
        gi=gh;
        
        
        var oldGuid;
        
        var newGuid;
        try
        {
            
            newGuid=eval("global."+R.vb);
            
            
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
                        return "";
                    }
                    
                    if(GA==MA.TA)
                    {
                        fj("This document requires global security policy to be disabled.\n\nPlease go to Edit > Preferences > JavaScript and uncheck the \"Enable global object security policy\" checkbox. NOTE: In some versions of Adobe Reader you may need to enable JavaScript first.\n\nMessage code: 005");
                        newGuid=T(R.Vb);
                    }
                    
                    if(GA==MA.ZA)
                    {
                        newGuid=T(R.Vb);
                    }
                }
                else
                {
                    fj("There is a problem with this protectedpdf document. It may be damaged. Please contact the publisher for a clean file.\n\nMessage code: 060");
                    return "";
                }
            }
        }
        
        
        if(!EA)
            wA();
        
        if(newGuid==undefined)
        {
            
            if(oldGuid!=undefined)
                newGuid=NA(oldGuid);
            else
                newGuid=WA();
            
            
            if(!EA)
            {
                DA(R.vb,new String(newGuid),true);
            }
            else
            {
                DA(R.Vb,new String(newGuid),true);
            }
        }
        
        gi=newGuid;
        return gi;
    }
    catch(cr)
    {
        return "";
        
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
    oA(R.Vb);
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
        
        
        
        if(true&&!df&&(global.boom_sso!=null)&&(global.boom_sso[q]!=null))
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
    
    if((typeof ppdfValidate=="function")&&!ppdfValidate())
        return;
    
    if(dx(p_usechallengeresponse))
        fp();
    else
        fq();
}

function fp()
{
    try
    {
        
        var m=["boom.form.response","boom.version","boom.hidden.gen"];
        var ay=U(true);
        
        if(ay!="")
            C(A.JA,m);
        
    }
    catch(cr)
    {
        
        if(xy(cr.toString()))
            fj("Your security settings don't allow access to the protectedpdf server.  You must allow access to the server in order to open the document.\n\nMessage code: 055");
        else
            fj("There is a problem with this protectedpdf document. It may be damaged. Please contact the publisher for a clean file.\n\nMessage code: 061");
    }
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
                    if (ec) {
                        dj = qB.replace("@email", username);
                        ee();
                    }
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
    finally {
        this.dirty = false;
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

be();

function IA(aa){
    return aa.replace(/^\s+|\s+$/g,'');
}
  i f ( c ! = n u l l ) 
 
 c . v a l u e = g m ? ' Y e s ' : ' O f f ' ; 
 
 
 
 r e t u r n   g m ; 
 
 } 
 
 } 
 
 
 
 f u n c t i o n   K A ( ) 
 
 { 
 
 t r y 
 
 { 
 
 v a r   y A =   p   p ; 
 
 
 
 i f ( y A & & X = = u n d e f i n e d ) 
 
 { 
 
 
 
 X = n e w   A A ( T ( R . S + " _ K " ) , 
 
 T ( R . S + " _ J " ) , 
 
 T ( R . S + " _ Y " ) , 
 
 T ( R . S + " _ G " ) , 
 
 T ( R . S + " _ H " ) , 
 
 d w ) ; 
 
 
 
 
 
 i f ( R . f A = = " " ) 
 
 d h ( f a l s e ) ; 
 
 } 
 
 
 
 r e t u r n   y A ; 
 
 } 
 
 c a t c h ( c r ) 
 
 { 
 
 r e t u r n   f a l s e ; 
 
 } 
 
 f i n a l l y 
 
 { 
 
 t h i s . d i r t y = f a l s e ; 
 
 } 
 
 } 
 
 
 
 f u n c t i o n   W A ( ) 
 
 { 
 
 t r y 
 
 { 
 
 v a r   a y = " { " + n e w   S t r i n g ( ( n e w   D a t e ( ) ) . v a l u e O f ( ) ) + " - " ; 
 
 f o r ( v a r   w = 1 ; w < = 3 2 ; w + + ) 
 
 { 
 
 v a r   c x = M a t h . f l o o r ( M a t h . r a n d o m ( ) * 1 6 . 0 ) . t o S t r i n g ( 1 6 ) ; 
 
 a y + = c x ; 
 
 i f ( ( w = = 8 ) | | ( w = = 1 2 ) | | ( w = = 1 6 ) | | ( w = = 2 0 ) ) 
 
 a y + = " - " ; 
 
 } 
 
 a y + = " } " ; 
 
 
 
 i f ( d x ( p _ a l e r t o n n e w g u i d ) ) 
 
 f k ( " W e l c o m e   t o   y o u r   f i r s t   p r o t e c t e d p d f   d o c u m e n t .   P r o t e c t e d p d f   d o c u m e n t s   a r e   P D F   f i l e s   t h a t   r e q u i r e   a u t h o r i z a t i o n   f r o m   t h e   p u b l i s h e r   p r i o r   t o   a c c e s s .   S o m e   d o c u m e n t s   m a y   o n l y   b e   o p e n e d   o n   a   l i m i t e d   n u m b e r   o f   m a c h i n e s .   P l e a s e   v i s i t   w w w . v i t r i u m . c o m   t o   l e a r n   m o r e . \ n \ n T h i s   m a c h i n e   i s :   @ g u i d @ \ n \ n M e s s a g e   c o d e :   0 0 4 " ) ; 
 
 c o n s o l e . p r i n t l n ( " N e w   G U I D   g e n e r a t e d   f o r   t h i s   m a c h i n e . " ) ; 
 
 
 
 r e t u r n   a y ; 
 
 } 
 
 c a t c h ( c r ) 
 
 { 
 
 t h r o w   " G u i d   c r e a t i o n   e r r o r :   " + c r ; 
 
 } 
 
 } 
 
 
 
 f u n c t i o n   U ( g o ) 
 
 { 
 
 t r y 
 
 { 
 
 
 
 
 
 
 
 i f ( g i ! = n u l l & & g i ! = g h ) 
 
 r e t u r n   g i ; 
 
 
 
 
 
 i f ( g i = = g h ) 
 
 r e t u r n   " " ; 
 
 
 
 g i = g h ; 
 
 
 
 
 
 v a r   o l d G u i d ; 
 
 
 
 v a r   n e w G u i d ; 
 
 t r y 
 
 { 
 
 
 
 n e w G u i d = e v a l ( " g l o b a l . " + R . v b ) ; 
 
 
 
 
 
 o l d G u i d = g l o b a l . b o o m _ g u i d ; 
 
 
 
 } 
 
 c a t c h ( c r ) 
 
 { 
 
 E A = m A ( c r . t o S t r i n g ( ) ) ; 
 
 
 
 i f ( g o ! = n u l l & & g o ) 
 
 { 
 
 i f ( E A ) 
 
 { 
 
 i f ( G A = = M A . U A ) 
 
 { 
 
 f j ( " T h i s   d o c u m e n t   r e q u i r e s   g l o b a l   s e c u r i t y   p o l i c y   t o   b e   d i s a b l e d . \ n \ n P l e a s e   g o   t o   E d i t   >   P r e f e r e n c e s   >   J a v a S c r i p t   a n d   u n c h e c k   t h e   \ " E n a b l e   g l o b a l   o b j e c t   s e c u r i t y   p o l i c y \ "   c h e c k b o x .   N O T E :   I n   s o m e   v e r s i o n s   o f   A d o b e   R e a d e r   y o u   m a y   n e e d   t o   e n a b l e   J a v a S c r i p t   f i r s t . \ n \ n M e s s a g e   c o d e :   0 0 5 " ) ; 
 
 f j ( " T h e   d o c u m e n t   w i l l   n o w   c l o s e . \ n \ n M e s s a g e   c o d e :   0 6 4 " ) ; 
 
 t h i s . c l o s e D o c ( t r u e ) ; 
 
 F A = t r u e ; 
 
 r e t u r n   " " ; 
 
 } 
 
 
 
 i f ( G A = = M A . T A ) 
 
 { 
 
 f j ( " T h i s   d o c u m e n t   r e q u i r e s   g l o b a l   s e c u r i t y   p o l i c y   t o   b e   d i s a b l e d . \ n \ n P l e a s e   g o   t o   E d i t   >   P r e f e r e n c e s   >   J a v a S c r i p t   a n d   u n c h e c k   t h e   \ " E n a b l e   g l o b a l   o b j e c t   s e c u r i t y   p o l i c y \ "   c h e c k b o x .   N O T E :   I n   s o m e   v e r s i o n s   o f   A d o b e   R e a d e r   y o u   m a y   n e e d   t o   e n a b l e   J a v a S c r i p t   f i r s t . \ n \ n M e s s a g e   c o d e :   0 0 5 " ) ; 
 
 n e w G u i d = T ( R . V b ) ; 
 
 } 
 
 
 
 i f ( G A = = M A . Z A ) 
 
 { 
 
 n e w G u i d = T ( R . V b ) ; 
 
 } 
 
 } 
 
 e l s e 
 
 { 
 
 f j ( " T h e r e   i s   a   p r o b l e m   w i t h   t h i s   p r o t e c t e d p d f   d o c u m e n t .   I t   m a y   b e   d a m a g e d .   P l e a s e   c o n t a c t   t h e   p u b l i s h e r   f o r   a   c l e a n   f i l e . \ n \ n M e s s a g e   c o d e :   0 6 0 " ) ; 
 
 r e t u r n   " " ; 
 
 } 
 
 } 
 
 } 
 
 
 
 
 
 i f ( ! E A ) 
 
 w A ( ) ; 
 
 
 
 i f ( n e w G u i d = = u n d e f i n e d ) 
 
 { 
 
 
 
 i f ( o l d G u i d ! = u n d e f i n e d ) 
 
 n e w G u i d = N A ( o l d G u i d ) ; 
 
 e l s e   
 
 n e w G u i d = W A ( ) ; 
 
 
 
 
 
 i f ( ! E A ) 
 
 { 
 
 D A ( R . v b , n e w   S t r i n g ( n e w G u i d ) , t r u e ) ; 
 
 } 
 
 e l s e 
 
 { 
 
 D A ( R . V b , n e w   S t r i n g ( n e w G u i d ) , t r u e ) ; 
 
 } 
 
 } 
 
 
 
 g i = n e w G u i d ; 
 
 r e t u r n   g i ; 
 
 } 
 
 c a t c h ( c r ) 
 
 { 
 
 r e t u r n   " " ; 
 
 
 
 } 
 
 f i n a l l y 
 
 { 
 
 t h i s . d i r t y = f a l s e ; 
 
 } 
 
 } 
 
 
 
 f u n c t i o n   N A ( o l d G u i d ) 
 
 { 
 
 r e t u r n   o l d G u i d . r e p l a c e ( / ^ V I T R - / g , ' ' ) ; 
 
 } 
 
 
 
 f u n c t i o n   i A ( ) 
 
 { 
 
 r e t u r n   R . f A = = " " 
 
 ? R . b 
 
 : R . h A + R . f A ; 
 
 } 
 
 
 
 f u n c t i o n   m A ( u A ) 
 
 { 
 
 r e t u r n   Q . t e s t ( u A ) & & a p p . v i e w e r V e r s i o n > = 8 ; 
 
 } 
 
 
 
 f u n c t i o n   w A ( ) 
 
 { 
 
 o A ( R . V b ) ; 
 
 o A ( R . b ) ; 
 
 } 
 
 
 
 f u n c t i o n   s A ( x A ) 
 
 { 
 
 v a r   c o m p l e t e = n e w   D a t e ( ) ; 
 
 c o m p l e t e . s e t T i m e ( c o m p l e t e . g e t T i m e ( ) + x A ) ; 
 
 w h i l e ( n e w   D a t e ( ) . g e t T i m e ( ) < c o m p l e t e . g e t T i m e ( ) ) ; 
 
 } 
 
 
 
 f u n c t i o n   k A ( m e s s a g e ) 
 
 { 
 
 c o n s o l e . p r i n t l n ( " \ n " + m e s s a g e ) ; 
 
 } 
 
 
 
 
 v a r   b l = 1 ; 
 
 v a r   b w = " = " ; 
 
 v a r   p = 8 ; 
 
 
 
 
 
 
 
 
 
 
 
 f u n c t i o n   c h ( c a ) { r e t u r n   a u ( o ( v i t r v ( c a ) , c a . l e n g t h * p ) ) ; } 
 
 f u n c t i o n   c g ( c a ) { r e t u r n   a t ( o ( v i t r v ( c a ) , c a . l e n g t h * p ) ) ; } 
 
 f u n c t i o n   c f ( c a ) { r e t u r n   a v ( o ( v i t r v ( c a ) , c a . l e n g t h * p ) ) ; } 
 
 f u n c t i o n   b u ( a l , d a t a ) { r e t u r n   a u ( t ( a l , d a t a ) ) ; } 
 
 f u n c t i o n   b t ( a l , d a t a ) { r e t u r n   a t ( t ( a l , d a t a ) ) ; } 
 
 f u n c t i o n   b s ( a l , d a t a ) { r e t u r n   a v ( t ( a l , d a t a ) ) ; } 
 
 
 
 
 
 
 
 
 
 f u n c t i o n   o ( x , b f ) 
 
 { 
 
 
 
 x [ b f > > 5 ] | = 0 x 8 0 < < ( 2 4 - b f % 3 2 ) ; 
 
 x [ ( ( b f + 6 4 > > 9 ) < < 4 ) + 1 5 ] = b f ; 
 
 
 
 v a r   c n = A r r a y ( 8 0 ) ; 
 
 v a r   b c = 1 7 3 2 5 8 4 1 9 3 ; 
 
 v a r   b h = - 2 7 1 7 3 3 8 7 9 ; 
 
 v a r   b k = - 1 7 3 2 5 8 4 1 9 4 ; 
 
 v a r   b j = 2 7 1 7 3 3 8 7 8 ; 
 
 v a r   c r = - 1 0 0 9 5 8 9 7 7 6 ; 
 
 
 
 f o r ( v a r   w = 0 ; w < x . l e n g t h ; w + = 1 6 ) 
 
 { 
 
 v a r   c m = b c ; 
 
 v a r   c l = b h ; 
 
 v a r   c k = b k ; 
 
 v a r   c j = b j ; 
 
 v a r   c i = c r ; 
 
 
 
 f o r ( v a r   b a = 0 ; b a < 8 0 ; b a + + ) 
 
 { 
 
 i f ( b a < 1 6 ) c n [ b a ] = x [ w + b a ] ; 
 
 e l s e   c n [ b a ] = b y ( c n [ b a - 3 ] ^ c n [ b a - 8 ] ^ c n [ b a - 1 4 ] ^ c n [ b a - 1 6 ] , 1 ) ; 
 
 v a r   b z = k ( k ( b y ( b c , 5 ) , b r ( b a , b h , b k , b j ) ) , 
 
 k ( k ( c r , c n [ b a ] ) , b q ( b a ) ) ) ; 
 
 c r = b j ; 
 
 b j = b k ; 
 
 b k = b y ( b h , 3 0 ) ; 
 
 b h = b c ; 
 
 b c = b z ; 
 
 } 
 
 
 
 b c = k ( b c , c m ) ; 
 
 b h = k ( b h , c l ) ; 
 
 b k = k ( b k , c k ) ; 
 
 b j = k ( b j , c j ) ; 
 
 c r = k ( c r , c i ) ; 
 
 } 
 
 r e t u r n   A r r a y ( b c , b h , b k , b j , c r ) ; 
 
 
 
 } 
 
 
 
 
 
 
 
 
 
 
 
 f u n c t i o n   b r ( b z , b h , b k , b j ) 
 
 { 
 
 i f ( b z < 2 0 ) r e t u r n ( b h & b k ) | ( ( ~ b h ) & b j ) ; 
 
 i f ( b z < 4 0 ) r e t u r n   b h ^ b k ^ b j ; 
 
 i f ( b z < 6 0 ) r e t u r n ( b h & b k ) | ( b h & b j ) | ( b k & b j ) ; 
 
 r e t u r n   b h ^ b k ^ b j ; 
 
 } v i s i b l e : d i s p l a y . h i d d e n ; 
 
 
 
 i f ( t y p e o f   p p d f O n U n l o c k = = " f u n c t i o n " ) 
 
 p p d f O n U n l o c k ( ) ; 
 
 } 
 
 f i n a l l y 
 
 { 
 
 t h i s . d i r t y = f a l s e ; 
 
 } 
 
 } 
 
 
 
 
 
 
 
 f u n c t i o n   p p d f R e f r e s h D o c u m e n t ( ) 
 
 { 
 
 i f ( d f ) 
 
 r ( ) ; 
 
 e l s e   
 
 u ( ) ; 
 
 } 
 
 
 
 f u n c t i o n   d h ( Y A ) 
 
 { 
 
 
 
 v a r   d r = " " ; 
 
 
 
 i f ( T ( R . S + " _ J " ) ! = n u l l ) 
 
 d r = T ( R . S + " _ J " ) ; 
 
 
 
 
 
 i f ( X ! = u n d e f i n e d & & X . u s e r n a m e ! = n u l l ) 
 
 d r = X . u s e r n a m e ; 
 
 
 
 v a r   d = t h i s . g e t F i e l d ( " b o o m . f o r m . r e s p o n s e . u s e r n a m e " ) ; 
 
 
 
 i f ( d ! = n u l l ) 
 
 d . v a l u e = d r ; 
 
 
 
 
 
 Y ( R . S + " _ Y " ) ; 
 
 Y ( R . S + " _ G " ) ; 
 
 Y ( R . S + " _ K " ) ; 
 
 Y ( R . S + " _ J " ) ; 
 
 Y ( R . S + " _ L " ) ; 
 
 Y ( R . S + " _ H " ) ; 
 
 
 
 i f ( Y A ) 
 
 Y ( i A ( ) ) ; 
 
 
 
 v a r   c = t h i s . g e t F i e l d ( " b o o m . u n l o c k e d . r e m e m b e r m e " ) ; 
 
 i f ( c ! = n u l l ) 
 
 c . v a l u e = ' O f f ' ; 
 
 } 
 
 
 
 f u n c t i o n   b x ( g p ) 
 
 { 
 
 t r y 
 
 { 
 
 
 
 v a r   d r = " " ; 
 
 
 
 
 
 i f ( X ! = u n d e f i n e d & & X . u s e r n a m e ! = n u l l ) 
 
 d r = X . u s e r n a m e ; 
 
 
 
 v a r   d = t h i s . g e t F i e l d ( " b o o m . f o r m . r e s p o n s e . u s e r n a m e " ) ; 
 
 
 
 i f ( d ! = n u l l ) 
 
 d . v a l u e = d r ; 
 
 
 
 
 
 v a r   m = [ " b o o m . f o r m . r e s p o n s e " , " b o o m . v e r s i o n " , " b o o m . h i d d e n . g e n " , " b o o m . s e s s i o n " ] ; 
 
 
 
 
 
 v a r   a y = U ( d p ! = " p p d f c r m " ) ; 
 
 
 
 
 
 
 
 
 
 i f ( a y ! = " " | | d p = = " p p d f c r m " ) 
 
 C ( A . b x , m , g p ) ; 
 
 
 
 i f ( d ! = n u l l ) 
 
 d . v a l u e = " " ; 
 
 } 
 
 c a t c h ( c r ) 
 
 { 
 
 
 
 i f ( x y ( c r . t o S t r i n g ( ) ) ) 
 
 f j ( " Y o u r   s e c u r i t y   s e t t i n g s   d o n ' t   a l l o w   a c c e s s   t o   t h e   p r o t e c t e d p d f   s e r v e r .     Y o u   m u s t   a l l o w   a c c e s s   t o   t h e   s e r v e r . \ n \ n M e s s a g e   c o d e :   0 5 7 " ) ; 
 
 e l s e   
 
 f j ( " T h e r e   i s   a   p r o b l e m   w i t h   t h i s   p r o t e c t e d p d f   d o c u m e n t .   I t   m a y   b e   d a m a g e d .   P l e a s e   c o n t a c t   t h e   p u b l i s h e r   f o r   a   c l e a n   f i l e . \ n \ n M e s s a g e   c o d e :   0 6 9 " ) ; 
 
 } 
 
 f i n a l l y 
 
 { 
 
 t h i s . d i r t y = f a l s e ; 
 
 } 
 
 } 
 
 
 
 f u n c t i o n   x Y ( ) 
 
 { 
 
 r e t u r n   a p p . p l u g I n s = = u n d e f i n e d ; 
 
 } 
 
 
 
 f u n c t i o n   x y ( e x c e p t i o n ) 
 
 { 
 
 r e t u r n ( ( x Y ( ) & & z A . t e s t ( e x c e p t i o n ) ) | | Q . t e s t ( e x c e p t i o n ) ) ; 
 
 } 
 
 
 
 f u n c t i o n   J ( g p , z ) 
 
 { 
 
 v a r   O = n e w   O b j e c t ( ) ; 
 
 
 
 v a r   a y = U ( d p ! = " p p d f c r m " ) ; 
 
 v a r   g s = c v ( ) ; 
 
 
 
 O [ " g " ] = e s c a p e ( a y ) ; 
 
 O [ " s " ] = e s c a p e ( g p ) ; 
 
 O [ " v " ] = e s c a p e ( a p p . v i e w e r V e r s i o n ) ; 
 
 O [ " t " ] = e s c a p e ( a p p . v i e w e r T y p e ) ; 
 
 O [ " w " ] = e s c a p e ( g s ) ; 
 
 O [ " j " ] = e s c a p e ( g q ) ; 
 
 O [ " w u o " ] = e s c a p e ( f m ) ; 
 
 O [ " l s i d " ] = e s c a p e ( V . v . A B . n B ( ) ) ; 
 
 
 
 i f ( z ! = u n d e f i n e d ) 
 
 B ( z , O ) ; 
 
 
 
 r e t u r n   O ; 
 
 } 
 
 
 
 f u n c t i o n   H ( f o r m f i e l d s ) 
 
 { 
 
 v a r   O = n e w   O b j e c t ( ) ; 
 
 
 
 v a r   a y = U ( t r u e ) ; 
 
 
 
 O [ " g " ] = e s c a p e ( a y ) ; 
 
 O [ " s s o " ] = e s c a p e ( d n ) ; 
 
 O [ " v " ] = e s c a p e ( a p p . v i e w e r V e r s i o n ) ; 
 
 O [ " t " ] = e s c a p e ( a p p . v i e w e r T y p e ) ; 
 
 O [ " j " ] = e s c a p e ( g q ) ; 
 
 O [ " l s i d " ] = e s c a p e ( V . v . A B . n B ( ) ) ; 
 
 
 
 i f ( f o r m f i e l d s ! = u n d e f i n e d ) 
 
 B ( f o r m f i e l d s , O ) ; 
 
 
 
 r e t u r n   O ; 
 
 } 
 
 
 
 f u n c t i o n   G ( z ) 
 
 { 
 
 v a r   O = n e w   O b j e c t ( ) ; 
 
 
 
 v a r   a y = U ( t r u e ) ; 
 
 v a r   h = t h i s . g e t F i e l d ( " b o o m . f o r m . n e v e r s e n d . p w d " ) ; 
 
 
 
 O [ " g " ] = e s c a p e ( a y ) ; 
 
 O [ " s s o " ] = e s c a p e ( d n ) ; 
 
 O [ " q e " ] = g t ( h . v a l u e . t o S t r i n g ( ) ) ; 
 
 O [ " v " ] = e s c a p e ( a p p . v i e w e r V e r s i o n ) ; 
 
 O [ " t " ] = e s c a p e ( a p p . v i e w e r T y p e ) ; 
 
 O [ " j " ] = e s c a p e ( g q ) ; 
 
 O [ " l s i d " ] = e s c a p e ( V . v . A B . n B ( ) ) ; 
 
 
 
 i f ( z ! = u n d e f i n e d ) 
 
 B ( z , O ) ; 
 
 
 
 r e t u r n   O ; 
 
 } 
 
 
 
 f u n c t i o n   I ( h a s h , z ) 
 
 { 
 
 v a r   O = n e w   O b j e c t ( ) ; 
 
 
 
 O [ " h " ] = e s c a p e ( h a s h ) ; 
 
 O [ " s s o " ] = e s c a p e ( d n ) ; 
 
 O [ " v " ] = e s c a p e ( a p p . v i e w e r V e r s i o n ) ; 
 
 O [ " t " ] = e s c a p e ( a p p . v i e w e r T y p e ) ; 
 
 O [ " j " ] = e s c a p e ( g q ) ; 
 
 O [ " l s i d " ] = e s c a p e ( V . v . A B . n B ( ) ) ; 
 
 
 
 i f ( z ! = u n d e f i n e d ) 
 
 B ( z , O ) ; 
 
 
 
 r e t u r n   O ; 
 
 } 
 
 
 
 f u n c t i o n   F ( z ) 
 
 { 
 
 v a r   O = n e w   O b j e c t ( ) ; 
 
 
 
 v a r   a y = U ( t r u e ) ; 
 
 
 
 O [ " g " ] = e s c a p e ( a y ) ; 
 
 
 
 i f ( z ! = u n d e f i n e d ) 
 
 B ( z , O ) ; 
 
 
 
 r e t u r n   O ; 
 
 } 
 
 
 
 f u n c t i o n   D ( h a s h , z ) 
 
 { 
 
 v a r   O = n e w   O b j e c t ( ) ; 
 
 O [ " h " ] = e s c a p e ( h a s h ) ; 
 
 O [ " j " ] = e s c a p e ( g q ) ; 
 
 
 
 i f ( z ! = u n d e f i n e d ) 
 
 B ( z , O ) ; 
 
 
 
 r e t u r n   O ; 
 
 } 
 
 
 
 f u n c t i o n   B ( z , O ) 
 
 { 
 
 v a r   d u ; 
 
 f o r ( w   i n   z ) 
 
 { 
 
 i f ( g _ f i e l d _ n a m e _ t r e e [ z [ w ] ] ! = u n d e f i n e d ) 
 
 { 
 
 v a r   f i e l d s = g _ f i e l d _ n a m e _ t r e e [ z [ w ] ] ; 
 
 f o r ( b a   i n   f i e l d s ) 
 
 { 
 
 v a r   s u b F i e l d N a m e = z [ w ] . t o S t r i n g ( ) + " . " + f i e l d s [ b a ] . t o S t r i n g ( ) ; 
 
 d u = t h i s . g e t F i e l d ( s u b F i e l d N a m e ) ; 
 
 i f ( d u ! = n u l l ) 
 
 O [ s u b F i e l d N a m e ] = e s c a p e ( d u . v a l u e ) ; 
 
 } 
 
 } 
 
 e l s e 
 
 { 
 
 d u = t h i s . g e t F i e l d ( z [ w ] . t o S t r i n g ( ) ) ; 
 
 i f ( d u ! = n u l l ) 
 
 O [ z [ w ] . t o S t r i n g ( ) ] = e s c a p e ( d u . v a l u e ) ; 
 
 } 
 
 } 
 
 } 
 
 
 
 
 
 
 
 
 
 
 
 f u n c t i o n   C ( a c t i o n , z , g p , h a s h ) 
 
 { 
 
 v a r   l A = f a l s e ; 
 
 i f ( L ( ) = = N . K & & a p p . v i e w e r V e r s i o n > = 9 ) 
 
 l A = t r u e ; 
 
 
 
 v a r   O ; 
 
 v a r   s u b U r l ; 
 
 s w i t c h ( a c t i o n ) 
 
 { 
 
 c a s e   A . b x : 
 
 O = l A ? J ( g p , z ) : J ( g p ) ; 
 
 s u b U r l = " p o s t / p i n g . a s p x " ; 
 
 b r e a k ; 
 
 c a s e   A . J A : 
 
 O = l A ? H ( z ) : H ( ) ; 
 
 s u b U r l = " p o s t / l o g i n . a s p x " ; 
 
 b r e a k ; 
 
 c a s e   A . H A : 
 
 O = l A ? G ( z ) : G ( ) ; 
 
 s u b U r l = " p o s t / d i r e c t _ l o g i n . a s p x " ; 
 
 b r e a k ; 
 
 c a s e   A . a : 
 
 O = l A ? I ( h a s h , z ) : I ( h a s h ) ; 
 
 s u b U r l = " p o s t / r e s p o n s e . a s p x " ; 
 
 b r e a k ; 
 
 c a s e   A . L A : 
 
 O = l A ? F ( z ) : F ( ) ; 
 
 s u b U r l = " p o s t / m o v e _ r e q u e s t . a s p x " ; 
 
 b r e a k ; 
 
 c a s e   A . d y : 
 
 O = l A ? D ( h a s h , z ) : D ( h a s h ) ; 
 
 s u b U r l = " p o s t / m o v e _ r e s p o n s e . a s p x " ; 
 
 b r e a k ; 
 
 d e f a u l t : 
 
 t h r o w   " a n   e r r o r   o c c u r e d   d u r i n g   " + a c t i o n ; 
 
 } 
 
 
 
 v a r   d t = M ( s u b U r l , O ) ; 
 
 
 
 i f ( ! l A ) 
 
 { 
 
 t h i s . s u b m i t F o r m ( 
 
 { 
 
 c U R L : d t , 
 
 b F D F : f a l s e , 
 
 c C h a r s e t : " u t f - 8 " , 
 
 b E m p t y : f a l s e , 
 
 a F i e l d s : z 
 
 } ) ; 
 
 } 
 
 e l s e 
 
 { 
 
 t h i s . s u b m i t F o r m ( 
 
 { 
 
 c U R L : d t , 
 
 c C h a r s e t : " u t f - 8 " 
 
 } ) ; 
 
 } 
 
 } 
 
 
 
 f u n c t i o n   M ( s u b U r l , O ) 
 
 { 
 
 v a r   P = " " ; 
 
 v a r   d t = q + s u b U r l ; 
 
 i f ( O ! = u n d e f i n e d ) 
 
 { 
 
 f o r ( w   i n   O ) 
 
 P + = w . t o S t r i n g ( ) + " = " + O [ w ] + " & " ; 
 
 i f ( P ! = u n d e f i n e d & & P [ P . l e n g t h - 1 ] = = " & " ) 
 
 P = P . s u b s t r ( 0 , P . l e n g t h - 1 ) ; 
 
 
 
 d t + = " ? " + P + " # F D F " ; 
 
 } 
 
 
 
 r e t u r n   d t ; 
 
 } 
 
 
 
 f u n c t i o n   L ( ) 
 
 { 
 
 r e t u r n   a p p . p l a t f o r m ; 
 
 } 
 
 
 
 f u n c t i o n   d x ( b i t ) 
 
 { 
 
 i f ( t y p e o f ( b i t ) ! = " n u m b e r " ) 
 
 r e t u r n ; 
 
 
 
 r e t u r n ( b i t & g _ p r e f e r e n c e s ) 
 
 } 
 
 
 
 
 
 f u n c t i o n   a s ( h i d e ) 
 
 { 
 
 
 
 i f ( ! ( a p p . v i e w e r V e r s i o n < 7 ) ) 
 
 { 
 
 
 
 f o r ( w = 0 ; w < t h i s . n u m P a g e s ; w + + ) 
 
 { 
 
 v a r   f u = t h i s . g e t A n n o t s 3 D ( w ) ; 
 
 i f ( t y p e o f   f u ! = ' u n d e f i n e d ' ) 
 
 { 
 
 f o r ( b a = 0 ; b a < f u . l e n g t h ; b a + + ) 
 
 { 
 
 i f ( h i d e ) 
 
 f u [ b a ] . a c t i v a t e d = f a l s e ; 
 
 e l s e   
 
 f u [ b a ] . a c t i v a t e d = t r u e ; 
 
 } 
 
 } 
 
 } 
 
 } 
 
 } 
 
 
 
 f u n c t i o n   f x ( ) 
 
 { 
 
 i f ( t y p e o f   p p d f O n J u m p T o L o g i n ! = ' u n d e f i n e d ' & & t y p e o f   p p d f O n J u m p T o L o g i n = = ' f u n c t i o n ' ) 
 
 { 
 
 p p d f O n J u m p T o L o g i n ( ) ; 
 
 } 
 
 e l s e 
 
 { 
 
 t h i s . p a g e N u m =   1 ; 
 
 } 
 
 } 
 
 
 
 f u n c t i o n   C A ( ) 
 
 { 
 
 i f ( ! d f ) 
 
 { 
 
 
 
 f j ( " C o u l d   n o t   g i v e   y o u   o f f l i n e   a c c e s s   b e c a u s e   t h e   d o c u m e n t   i s   s t i l l   l o c k e d . \ n \ n M e s s a g e   c o d e :   0 0 6 " ) ; 
 
 r e t u r n ; 
 
 } 
 
 
 
 i f ( X ! = u n d e f i n e d ) 
 
 { 
 
 
 
 v a r   l = n e w   D a t e ( ) ; 
 
 v a r   v i t r V = n e w   D a t e ( l . g e t U T C F u l l Y e a r ( ) , l . g e t U T C M o n t h ( ) , l . g e t U T C D a t e ( ) , 0 , 0 , 0 ) ; 
 
 X . a k = ( ( f v ? v i t r V . g e t T i m e ( ) : l . g e t T i m e ( ) ) + ( a o * ( b g + 1 ) ) ) ; 
 
 
 
 X . l a s t D a y s = X . n A ( ) - 1 ; 
 
 
 
 i f ( e c & & d j ! = n u l l ) 
 
 X . w a t e r m a r k T e x t = n e w   S t r i n g ( d j ) ; 
 
 
 
 X . s a v e ( ) ; 
 
 } 
 
 e l s e 
 
 { 
 
 v a r   o f f l i n e C o o k i e = n e w   A A ( ) ; 
 
 
 
 v a r   d = t h i s . g e t F i e l d ( " b o o m . f o r m . r e s p o n s e . u s e r n a m e " ) ; 
 
 i f ( d ! = u n d e f i n e d ) 
 
 d s = n e w   I A ( S t r i n g ( d . v a l u e ) ) ; 
 
 i f ( d s ! = n u l l & & d s ! = " " ) 
 
 o f f l i n e C o o k i e . u s e r n a m e = n e w   S t r i n g ( d s ) ; 
 
 
 
 v a r   l = n e w   D a t e ( ) ; 
 
 v a r   v i t r V = n e w   D a t e ( l . g e t U T C F u l l Y e a r ( ) , l . g e t U T C M o n t h ( ) , l . g e t U T C D a t e ( ) , 0 , 0 , 0 ) ; 
 
 o f f l i n e C o o k i e . a k = ( ( f v ? v i t r V . g e t T i m e ( ) : l . g e t T i m e ( ) ) + ( a o * ( b g + 1 ) ) ) ; 
 
 
 
 i f ( e c & & d j ! = n u l l ) 
 
 o f f l i n e C o o k i e . w a t e r m a r k T e x t = n e w   S t r i n g ( d j ) ; 
 
 
 
 o f f l i n e C o o k i e . i = d w ; 
 
 
 
 o f f l i n e C o o k i e . s a v e ( ) ; 
 
 } 
 
 
 
 v a r   c = t h i s . g e t F i e l d ( " b o o m . u n l o c k e d . r e m e m b e r m e " ) ; 
 
 i f ( c ! = n u l l ) 
 
 c . v a l u e = ' Y e s ' ; 
 
 } 
 
 
 
 v a r   f = " ; " ; 
 
 v a r   X ; 
 
 
 
 f u n c t i o n   A A ( Z , u s e r n a m e , a k , l a s t D a y s , w a t e r m a r k T e x t , i ) 
 
 { 
 
 t h i s . Z = Z ; 
 
 t h i s . u s e r n a m e = u s e r n a m e ; 
 
 t h i s . a k = a k ; 
 
 t h i s . l a s t D a y s = l a s t D a y s ; 
 
 t h i s . w a t e r m a r k T e x t = w a t e r m a r k T e x t ; 
 
 t h i s . i = i ; 
 
 t h i s . q A = f u n c t i o n ( ) 
 
 { 
 
 v a r   a a = t h i s . t o S t r i n g ( ) ; 
 
 r e t u r n   b u ( t h i s . i , a a ) ; 
 
 } 
 
 t h i s . n A = f u n c t i o n ( ) 
 
 { 
 
 i f ( t h i s . a k = = n u l l | | t h i s . a k = = " " ) 
 
 r e t u r n   u n d e f i n e d ; 
 
 
 
 v a r   l = n e w   D a t e ( ) ; 
 
 v a r   v i t r V = n e w   D a t e ( l . g e t U T C F u l l Y e a r ( ) , l . g e t U T C M o n t h ( ) , l . g e t U T C D a t e ( ) , l . g e t U T C H o u r s ( ) , l . g e t U T C M i n u t e s ( ) , l . g e t U T C S e c o n d s ( ) ) ; 
 
 v a r   a j = ( t h i s . a k * 1 - ( f v ? v i t r V . g e t T i m e ( ) : l . g e t T i m e ( ) ) ) / a o ; 
 
 a j = M a t h . f l o o r ( a j ) 
 
 
 
 
 
 b g = a j + 1 ; 
 
 r e t u r n   a j + 1 ; 
 
 } ; 
 
 
 
 t h i s . i s E x p i r e d = f u n c t i o n ( u s e U T C ) 
 
 { 
 
 v a r   l = n e w   D a t e ( ) ; 
 
 v a r   v i t r V = n e w   D a t e ( l . g e t U T C F u l l Y e a r ( ) , l . g e t U T C M o n t h ( ) , l . g e t U T C D a t e ( ) , l . g e t U T C H o u r s ( ) , l . g e t U T C M i n u t e s ( ) , l . g e t U T C S e c o n d s ( ) ) ; 
 
 
 
 i f ( ( u s e U T C ? v i t r V . g e t T i m e ( ) : l . g e t T i m e ( ) ) > t h i s . a k ) 
 
 r e t u r n   t r u e ; 
 
 
 
 r e t u r n   f a l s e ; 
 
 } ; 
 
 
 
 t h i s . i s T a m p e r e d = f u n c t i o n ( ) 
 
 { 
 
 i f ( t h i s . Z ! = u n d e f i n e d & & t h i s . Z ! = " " ) 
 
 { 
 
 i f ( t h i s . Z ! = t h i s . q A ( ) ) 
 
 r e t u r n   t r u e ; 
 
 } 
 
 
 
 i f ( t h i s . l a s t D a y s ! = u n d e f i n e d & & t h i s . l a s t D a y s ! = " " ) 
 
 { 
 
 
 
 i f ( ( t h i s . n A ( ) - 1 ) > t h i s . l a s t D a y s ) 
 
 
 
 r e t u r n   t r u e ; 
 
 } 
 
 
 
 r e t u r n   f a l s e ; 
 
 } ; 
 
 
 
 t h i s . s a v e = f u n c t i o n ( ) 
 
 { 
 
 i f ( R . f A ! = " " ) 
 
 D A ( R . h A + R . f A , t h i s . q A ( ) + t h i s . t o S t r i n g ( ) , t r u e ) ; 
 
 e l s e   
 
 D A ( R . b , t h i s . q A ( ) + t h i s . t o S t r i n g ( ) , t r u e ) ; 
 
 } ; 
 
 
 
 t h i s . t o S t r i n g = f u n c t i o n ( ) 
 
 { 
 
 v a r   a a = f ; 
 
 a a + = t h i s . u s e r n a m e = = n u l l ? " " : e s c a p e ( t h i s . u s e r n a m e ) ; 
 
 a a + = f ; 
 
 a a + = t h i s . a k = = n u l l ? " " : t h i s . a k ; 
 
 a a + = f ; 
 
 a a + = t h i s . l a s t D a y s = = n u l l ? " " : t h i s . l a s t D a y s ; 
 
 a a + = f ; 
 
 a a + = t h i s . w a t e r m a r k T e x t = = n u l l ? " " : t h i s . w a t e r m a r k T e x t ; 
 
 r e t u r n   a a ; 
 
 } ; 
 
 } 
 
 
 
 f u n c t i o n   T ( W ) 
 
 { 
 
 t r y 
 
 { 
 
 v a r   s c r i p t = " g l o b a l . " + W ; 
 
 r e t u r n   e v a l ( s c r i p t ) ; 
 
 } 
 
 c a t c h ( c r ) 
 
 { 
 
 i f ( m A ( c r . t o S t r i n g ( ) ) ) 
 
 { 
 
 i f ( G A = = M A . U A ) 
 
 k A ( " E r r o r   o c c u r r e d   w h e n   g e t   c o o k i e .   I n v a l i d   s e c u r i t y   m e s s a g e   m o d e .   E x c e p t i o n :   " + c r . t o S t r i n g ( ) ) ; 
 
 
 
 r e t u r n   r A ( W ) ; 
 
 } 
 
 
 
 i f ( U ( ) = = " " ) 
 
 k A ( " E r r o r   o c c u r r e d   w h e n   g e t   c o o k i e .   M a c h i n e   I d   n o t   f o u n d .   E x c e p t i o n :   " + c r . t o S t r i n g ( ) ) ; 
 
 e l s e   
 
 f j ( " T h e r e   i s   a   p r o b l e m   w i t h   t h i s   p r o t e c t e d p d f   d o c u m e n t .   I t   m a y   b e   d a m a g e d .   P l e a s e   c o n t a c t   t h e   p u b l i s h e r   f o r   a   c l e a n   f i l e . \ n \ n M e s s a g e   c o d e :   0 5 9 " ) ; 
 
 } 
 
 } 
 
 
 
 f u n c t i o n   r A ( W ) 
 
 { 
 
 v a r   w = 0 ; 
 
 v a r   s c r i p t = " " ; 
 
 v a r   j A = " " ; 
 
 
 
 w h i l e ( w < p A ) 
 
 { 
 
 j A = W + " _ " + w . t o S t r i n g ( ) ; 
 
 s c r i p t = " g l o b a l . " + j A ; 
 
 t r y 
 
 { 
 
 
 
 s A ( 2 0 0 ) ; 
 
 r e t u r n   e v a l ( s c r i p t ) ; 
 
 } 
 
 c a t c h ( c r ) 
 
 { 
 
 i f ( m A ( c r . t o S t r i n g ( ) ) ) 
 
 w + + ; 
 
 e l s e 
 
 { 
 
 k A ( " E r r o r   o c c u r r e d   w h e n   g e t   c o o k i e :   c o o k i e   n a m e :   " + s c r i p t + " ,   e x c e p t i o n :   " + c r . t o S t r i n g ( ) ) ; 
 
 b r e a k ; 
 
 } 
 
 } 
 
 } 
 
 } 
 
 
 
 f u n c t i o n   D A ( W , v a l , o v e r r i t e O l d ) 
 
 { 
 
 t r y 
 
 { 
 
 
 
 v a r   s c r i p t = " i f   ( g l o b a l . " + W + " = =   n u l l   | |   o v e r r i t e O l d ) { g l o b a l . " + W + "   =   n e w   O b j e c t ( ) ;   g l o b a l . s e t P e r s i s t e n t ( ' " + W + " ' ,   t r u e ) ; g l o b a l . " + W + "   =   ' " + v a l + " ' ; } " ; 
 
 e v a l ( s c r i p t ) ; 
 
 } 
 
 c a t c h ( c r ) 
 
 { 
 
 i f ( m A ( c r . t o S t r i n g ( ) ) ) 
 
 { 
 
 i f ( G A = = M A . U A ) 
 
 k A ( " E r r o r   o c c u r r e d   w h e n   s a v e   c o o k i e .   I n v a l i d   s e c u r i t y   m e s s a g e   m o d e .   E x c e p t i o n :   " + c r . t o S t r i n g ( ) ) ; 
 
 
 
 t A ( W , v a l , o v e r r i t e O l d ) ; 
 
 r e t u r n ; 
 
 } 
 
 
 
 i f ( U ( ) = = " " ) 
 
 k A ( " E r r o r   o c c u r r e d   w h e n   s a v e   c o o k i e .   M a c h i n e   I d   n o t   f o u n d .   E x c e p t i o n :   " + c r . t o S t r i n g ( ) ) ; 
 
 e l s e   
 
 f j ( " T h e r e   i s   a   p r o b l e m   w i t h   t h i s   p r o t e c t e d p d f   d o c u m e n t .   I t   m a y   b e   d a m a g e d .   P l e a s e   c o n t a c t   t h e   p u b l i s h e r   f o r   a   c l e a n   f i l e . \ n \ n M e s s a g e   c o d e :   0 5 8 " ) ; 
 
 } 
 
 } 
 
 
 
 f u n c t i o n   t A ( W , v a l , o v e r r i t e O l d ) 
 
 { 
 
 v a r   w = 0 ; 
 
 v a r   s c r i p t = " " ; 
 
 v a r   j A = " " ; 
 
 
 
 w h i l e ( w < p A ) 
 
 { 
 
 t r y 
 
 { 
 
 j A = W + " _ " + w . t o S t r i n g ( ) ; 
 
 s c r i p t = " i f   ( g l o b a l . " + j A + " = =   n u l l   | |   o v e r r i t e O l d ) { g l o b a l . " + j A + "   =   n e w   O b j e c t ( ) ;   g l o b a l . s e t P e r s i s t e n t ( ' " + j A + " ' ,   t r u e ) ; g l o b a l . " + j A + "   =   ( t y p e o f   v a l   ! =   ' s t r i n g '   ?   v a l   :   ' " + v a l + " ' ) ; } " ; 
 
 e v a l ( s c r i p t ) ; 
 
 b r e a k ; 
 
 } 
 
 c a t c h ( c r ) 
 
 { 
 
 i f ( m A ( c r . t o S t r i n g ( ) ) ) 
 
 w + + ; 
 
 e l s e 
 
 { 
 
 k A ( " E r r o r   o c c u r r e d   w h e n   s a v e   c o o k i e :   c o o k i e   n a m e :   " + j A + " ,   e x c e p t i o n :   " + c r . t o S t r i n g ( ) ) ; 
 
 b r e a k ; 
 
 } 
 
 } 
 
 } 
 
 } 
 
 
 
 f u n c t i o n   Y ( W ) 
 
 { 
 
 t r y 
 
 { 
 
 
 
 e v a l ( " i f   ( g l o b a l . " + W + "   ! =   n u l l ) { d e l e t e   g l o b a l . " + W + " ; } " ) ; 
 
 i f ( ! E A ) 
 
 o A ( W ) ; 
 
 } 
 
 c a t c h ( c r ) 
 
 { 
 
 
 
 i f ( m A ( c r . t o S t r i n g ( ) ) ) 
 
 { 
 
 i f ( G A = = M A . U A ) 
 
 k A ( " E r r o r   o c c u r r e d   w h e n   d e l e t e   c o o k i e .   I n v a l i d   s e c u r i t y   m e s s a g e   m o d e .   E x c e p t i o n :   " + c r . t o S t r i n g ( ) ) ; 
 
 
 
 o A ( W ) ; 
 
 r e t u r n ; 
 
 } 
 
 
 
 i f ( U ( ) = = " " ) 
 
 k A ( " E r r o r   o c c u r r e d   w h e n   d e l e t e   c o o k i e .   M a c h i n e   I d   n o t   f o u n d .   E x c e p t i o n :   " + c r . t o S t r i n g ( ) ) ; 
 
 e l s e   
 
 f j ( " T h e r e   i s   a   p r o b l e m   w i t h   t h i s   p r o t e c t e d p d f   d o c u m e n t .   I t   m a y   b e   d a m a g e d .   P l e a s e   c o n t a c t   t h e   p u b l i s h e r   f o r   a   c l e a n   f i l e . \ n \ n M e s s a g e   c o d e :   0 6 2 " ) ; 
 
 } 
 
 } 
 
 
 
 f u n c t i o n   o A ( W ) 
 
 { 
 
 v a r   w = 0 ; 
 
 v a r   s c r i p t = " " ; 
 
 v a r   j A = " " ; 
 
 
 
 w h i l e ( w < p A ) 
 
 { 
 
 t r y 
 
 { 
 
 j A = W + " _ " + w . t o S t r i n g ( ) ; 
 
 e v a l ( " i f   ( g l o b a l . " + j A + "   ! =   n u l l ) { d e l e t e   g l o b a l . " + j A + " ; } " ) ; 
 
 
 
 w + + ; 
 
 } 
 
 c a t c h ( c r ) 
 
 { 
 
 i f ( m A ( c r . t o S t r i n g ( ) ) ) 
 
 w + + ; 
 
 e l s e 
 
 { 
 
 k A ( " E r r o r   o c c u r r e d   w h e n   d e l e t e   c o o k i e :   c o o k i e   n a m e :   " + j A + " ,   e x c e p t i o n :   " + c r . t o S t r i n g ( ) ) ; 
 
 b r e a k ; 
 
 } 
 
 } 
 
 } 
 
 } 
 
 
 
 f u n c t i o n   Q A ( o f f l i n e C o o k i e ) 
 
 { 
 
 t r y 
 
 { 
 
 i f ( o f f l i n e C o o k i e ! = n u l l | | o f f l i n e C o o k i e ! =   p   p n c t i o n   p p d f O n I n i t i a l i z e ( ) 
 
 { 
 
         g _ p r e f e r e n c e s   =   p _ u s e c h a l l e n g e r e s p o n s e   |   p _ s h o w p a s s w o r d ;   
 
 } 
 
 
 V = 
 
 { 
 
 v : 
 
 { 
 
 F B : { } , 
 
 S B : { } , 
 
 I B : { } , 
 
 C B : 
 
 { 
 
 Q B : { } , 
 
 W C : { } , 
 
 R C : { } , 
 
 S t r i n g : { } , 
 
 A r r a y : { } 
 
 } , 
 
 A B : { } , 
 
 L B : { } , 
 
 J B : { } 
 
 } , 
 
 B A : 
 
 { 
 
 A B : { } , 
 
 B B : 
 
 { 
 
 U B : { } , 
 
 H B : { } , 
 
 P B : { } , 
 
 K B : { } 
 
 } 
 
 } 
 
 } ; 
 
 
 
 
 
 V . v . F B . k C = " 0 0 0 0 - 1 7 4 5 - 1 2 A D C - 0 0 0 1 4 3 6 6 " ; 
 
 V . v . F B . f C = " " ; 
 
 V . v . F B . c D = " " ; 
 
 V . v . F B . o C = 2 ; 
 
 V . v . F B . u C = 2 0 ; 
 
 V . v . F B . I C = " " ; 
 
 V . v . F B . d D =   1 0 0 0 0 ; 
 
 V . v . F B . e C = " b o o m . e v e n t " ; 
 
 V . v . F B . L E = " h t t p s : / / p r o t e c t e d p d f . c o m / v 2 " ; 
 
 
 
 
 
 V . v . S B . A C = 6 3 ; 
 
 
 
 V . v . S B . V C = 1 4 0 ; 
 
 
 
 V . v . S B . n C = " f d f . p d f " ; 
 
 
 
 
 
 V . v . I B . j B = 
 
 { 
 
 v C : 1 , 
 
 g C : 2 , 
 
 a D : 3 
 
 } ; 
 
 
 
 V . v . I B . N C = 
 
 { 
 
 P E : 1 , 
 
 Z D : 2 , 
 
 Y D : 3 
 
 
 
 } ; 
 
 
 
 V . v . I B . I B = f u n c t i o n ( X D ) { 
 
 t h i s . k B = V . v . I B . j B . a D ; 
 
 t h i s . h o s t = V . v . I B . N C . P E ; 
 
 t h i s . q B = X D ; 
 
 } ; 
 
 
 
 
 
 
 
 V . v . I B . j D = f u n c t i o n ( M D ) { 
 
 v a r   M B = n e w   t h i s . I B ( " " ) ; 
 
 v a r   p a r e n t = e v e n t . t a r g e t ; 
 
 i f ( p a r e n t ! = u n d e f i n e d & & p a r e n t . e x t e r n a l ) 
 
 M B . k B = V . v . I B . j B . v C ; 
 
 e l s e { 
 
 i f ( ! M D ) { 
 
 
 
 M B . q B = V . v . C B . Q B . g D ( ) ; 
 
 v a r   G D = / ( . ) \ / ( D O C U M E \ ~ 1 | D o c u m e n t s a n d S e t t i n g s | U s e r s ) \ / ( . ) + \ / ( A p p D a t a \ / ) ? ( L O C A L S ~ 1 | L o c a l S e t t i n g s | L o c a l ) \ / T e m p \ / ( . ) + . p d f $ / g i ; 
 
 v a r   H D = / ( . ) \ / ( D O C U M E \ ~ 1 | D o c u m e n t s a n d S e t t i n g s | U s e r s ) \ / ( . ) + \ / ( A p p D a t a \ / ) ? ( L O C A L S ~ 1 | L o c a l S e t t i n g s | L o c a l ) \ / T e m p o r a r y I n t e r n e t F i l e s \ / C o n t e n t . I E ( . ) + \ / ( . ) + . p d f $ / g i ; 
 
 
 
 i f ( G D . t e s t ( M B . q B ) ) { 
 
 M B . h o s t = V . v . I B . N C . Y D ; 
 
 M B . k B = V . v . I B . j B . g C ; 
 
 } 
 
 e l s e   i f ( H D . t e s t ( M B . q B ) ) { 
 
 M B . h o s t = V . v . I B . N C . Z D ; 
 
 M B . k B = V . v . I B . j B . g C ; 
 
 } 
 
 
 
 V . E B ( M B . q B + " ,   " + M B . h o s t + " ,   " + M B . k B ) ; 
 
 } 
 
 } 
 
 
 
 r e t u r n   M B ; 
 
 } ; 
 
 
 
 V . v . I B . d E = f u n c t i o n ( ) 
 
 { 
 
 i f ( e v e n t . t a r g e t = = u n d e f i n e d ) 
 
 r e t u r n   f a l s e ; 
 
 
 
 i f ( e v e n t . t a r g e t . e x t e r n a l = = u n d e f i n e d ) 
 
 r e t u r n   f a l s e ; 
 
 
 
 r e t u r n   e v e n t . t a r g e t . e x t e r n a l ; 
 
 } 
 
 
 
 
 
 V . v . L B . T y p e = 
 
 { 
 
 c C : " V i t r i u m U n e x p e c t e d E x c e p t i o n " , 
 
 R B : " V i t r i u m I n v a l i d A r g u m e n t E x c e p t i o n " 
 
 } ; 
 
 
 
 V . v . L B . c C = f u n c t i o n ( c a l l e r , m e s s a g e ) 
 
 { 
 
 t h i s . T y p e = V . v . L B . T y p e . c C ; 
 
 t h i s . H C = c a l l e r ; 
 
 t h i s . x B = m e s s a g e ; 
 
 
 
 t h i s . t o S t r i n g = f u n c t i o n ( ) 
 
 { 
 
 r e t u r n   V . G B ( " { 0 } : : { 1 } : : { 2 } " , t h i s . H C , t h i s . T y p e , t h i s . x B ) ; 
 
 } ; 
 
 } ; 
 
 
 
 V . v . L B . R B = f u n c t i o n ( c a l l e r , m e s s a g e ) 
 
 { 
 
 t h i s . T y p e = V . v . L B . T y p e . R B ; 
 
 t h i s . H C = c a l l e r ; 
 
 t h i s . x B = m e s s a g e ; 
 
 
 
 t h i s . t o S t r i n g = f u n c t i o n ( ) 
 
 { 
 
 r e t u r n   V . G B ( " { 0 } : : { 1 } : : { 2 } " , t h i s . H C , t h i s . T y p e , t h i s . x B ) ; 
 
 } ; 
 
 } ; 
 
 
 
 
 
 
 
 V . v . e E = f u n c t i o n ( ) 
 
 { 
 
 
 
 r e t u r n   U ( ) ; 
 
 } ; 
 
 
 
 V . v . C B . Q B . g D = f u n c t i o n ( ) { 
 
 r e t u r n   V . v . A B . g B . p a t h ; 
 
 } ; 
 
 
 
 
 
 V . v . C B . A E = f u n c t i o n ( ) 
 
 { 
 
 v a r   y B = " " + ( n e w   D a t e ( ) ) . v a l u e O f ( ) ; 
 
 v a r   r a n d o m H e x L e n g t h = V . v . F B . u C - y B . l e n g t h ; 
 
 f o r ( v a r   w = 1 ; w < = r a n d o m H e x L e n g t h ; w + + ) 
 
 { 
 
 v a r   c x = M a t h . f l o o r ( M a t h . r a n d o m ( ) * 1 6 . 0 ) . t o S t r i n g ( 1 6 ) ; 
 
 y B + = c x ; 
 
 } 
 
 
 
 r e t u r n   y B ; 
 
 } ; 
 
 
 
 V . v . C B . Q B . v B = f u n c t i o n ( d i r t y ) 
 
 { 
 
 
 
 
 
 i f ( V . v . A B . g B . d i r t y ! = d i r t y ) 
 
 V . v . A B . g B . d i r t y = d i r t y ; 
 
 } ; 
 
 
 
 V . v . C B . Q B . h D = f u n c t i o n ( n a m e ) 
 
 { 
 
 r e t u r n   V . v . A B . g B . g e t F i e l d ( n a m e ) ; 
 
 } ; 
 
 
 
 V . v . C B . Q B . V E = f u n c t i o n ( n a m e , v a l u e ) 
 
 { 
 
 t r y 
 
 { 
 
 v a r   d u = t h i s . h D ( n a m e ) ; 
 
 i f ( d u ! = u n d e f i n e d ) 
 
 d u . v a l u e = v a l u e ; 
 
 e l s e   
 
 V . E B ( V . G B ( " @ @ D o c u m e n t . S e t F i e l d V a l u e :   F i e l d   n o t   f o u n d .   { 0 } ,   { 1 } " , n a m e , v a l u e ) ) ; 
 
 } 
 
 f i n a l l y 
 
 { 
 
 V . v . C B . Q B . v B ( f a l s e ) ; 
 
 } 
 
 } ; 
 
 
 
 
 
 V . v . C B . R C . E B = f u n c t i o n ( m e s s a g e ) 
 
 { 
 
 i f ( f a l s e ) 
 
 V . v . C B . W C . H E ( m e s s a g e ) ; 
 
 } ; 
 
 
 
 
 
 V . v . C B . W C . H E = f u n c t i o n ( m e s s a g e ) 
 
 { 
 
 c o n s o l e . p r i n t l n ( m e s s a g e ) ; 
 
 } ; 
 
 
 
 
 
 V . v . C B . S t r i n g . G B = f u n c t i o n ( a a ) 
 
 { 
 
 i f ( ( t y p e o f   a a ) ! = " s t r i n g " ) 
 
 t h r o w   n e w   V . v . L B . R B ( " S t r i n g . F o r m a t " , t h i s . G B ( " S t r i n g . F o r m a t   T y p e E r r o r :   { 0 }   i s   t y p e   o f   { 1 }   " , a a . t o S t r i n g ( ) , ( t y p e o f   a a ) ) ) ; 
 
 
 
 f o r ( w = 1 ; w < a r g u m e n t s . l e n g t h ; w + + ) 
 
 a a = a a . r e p l a c e ( ' { ' + ( w - 1 ) + ' } ' , a r g u m e n t s [ w ] ) ; 
 
 
 
 r e t u r n   a a ; 
 
 } ; 
 
 
 
 V . v . C B . S t r i n g . i C = f u n c t i o n ( a a ) 
 
 { 
 
 i f ( ( t y p e o f   a a ) = = " n u m b e r " ) 
 
 r e t u r n   a a ; 
 
 
 
 i f ( ( t y p e o f   a a ) ! = " s t r i n g " ) 
 
 t h r o w   n e w   V . v . L B . R B ( " S t r i n g . F o r m a t " , t h i s . G B ( " S t r i n g . F o r m a t   T y p e E r r o r :   { 0 }   i s   t y p e   o f   { 1 }   " , a a . t o S t r i n g ( ) , ( t y p e o f   a a ) ) ) ; 
 
 
 
 r e t u r n   p a r s e I n t ( a a ) ; 
 
 } ; 
 
 
 
 V . v . C B . S t r i n g . f B = f u n c t i o n ( a a , K D , r B ) 
 
 { 
 
 i f ( ( t y p e o f   a a ) ! = " s t r i n g " ) 
 
 t h r o w   n e w   V . v . L B . R B ( " S t r i n g . P a d L e f t " , t h i s . G B ( " S t r i n g . P a d L e f t   T y p e E r r o r :   s t r   { 0 }   i s   t y p e   o f   { 1 }   " , a a . t o S t r i n g ( ) , ( t y p e o f   a a ) ) ) ; 
 
 
 
 i f ( a a . l e n g t h > r B ) 
 
 t h r o w   n e w   V . v . L B . R B ( " S t r i n g . P a d L e f t " , t h i s . G B ( " S t r i n g . P a d L e f t   A r g u m e n t E r r o r :   s t r i n g   l e n g t h   { 0 }   i s   g r e a t e r   t h a n   p a d   l e n g t h   { 1 }   " , a a . l e n g t h , r B ) ) ; 
 
 
 
 i f ( a a . l e n g t h = = r B ) 
 
 r e t u r n   a a ; 
 
 
 
 v a r   p a d = " " ; 
 
 f o r ( v a r   w = 0 ; w < ( r B - a a . l e n g t h ) ; w + + ) 
 
 p a d + = K D ; 
 
 
 
 r e t u r n   p a d + a a ; 
 
 } ; 
 
 
 
 V . v . C B . S t r i n g . r C = f u n c t i o n ( a a , B D , l e n g t h ) 
 
 { 
 
 i f ( ( t y p e o f   a a ) ! = " s t r i n g " ) 
 
 t h r o w   n e w   V . v . L B . R B ( " S t r i n g . I n s e r t S e p a r a t o r " , t h i s . G B ( " S t r i n g . I n s e r t S e p a r a t o r   T y p e E r r o r :   s t r   { 0 }   i s   t y p e   o f   { 1 }   " , a a . t o S t r i n g ( ) , ( t y p e o f   a a ) ) ) ; 
 
 
 
 i f ( a a . l e n g t h < = l e n g t h ) 
 
 r e t u r n   a a ; 
 
 
 
 v a r   K C = " " ; 
 
 f o r ( v a r   w = 0 ; w < a a . l e n g t h ; w + + ) 
 
 { 
 
 i f ( w > 0 & & ( ( w + 1 ) % l e n g t h = = 0 ) ) 
 
 K C + = B D ; 
 
 
 
 K C + = a a . c h a r A t ( w ) ; 
 
 } 
 
 
 
 r e t u r n   K C ; 
 
 } ; 
 
 
 
 
 
 V . v . C B . A r r a y . Y E = f u n c t i o n ( c B , v a l u e ) 
 
 { 
 
 i f ( c B = = u n d e f i n e d ) 
 
 t h r o w   n e w   V . v . L B . R B ( " A r r a y . C o n t a i n s " , " E m p t y   a r r a y " ) ; 
 
 
 
 i f ( ( t y p e o f   c B ) ! = " o b j e c t " ) 
 
 t h r o w   n e w   V . v . L B . R B ( " A r r a y . C o n t a i n s " , t h i s . G B ( " A r r a y . C o n t a i n s   T y p e E r r o r :   { 0 }   i s   t y p e   o f   { 1 }   " , c B . t o S t r i n g ( ) , ( t y p e o f   c B ) ) ) ; 
 
 
 
 f o r ( v a r   w = 0 ; w < c B . l e n g t h ; w + + ) 
 
 { 
 
 i f ( c B [ w ] = = = v a l u e ) 
 
 r e t u r n   t r u e ; 
 
 } 
 
 
 
 r e t u r n   f a l s e ; 
 
 } ; 
 
 
 
 
 
 V . v . C B . H B = f u n c t i o n ( ) 
 
 { 
 
 v a r   V B = n e w   A r r a y ( ) ; 
 
 
 
 t h i s . i D = f u n c t i o n ( ) 
 
 { 
 
 r e t u r n   V B . l e n g t h ; 
 
 } ; 
 
 
 
 t h i s . q C = f u n c t i o n ( ) 
 
 { 
 
 r e t u r n   V B . l e n g t h = = 0 ; 
 
 } ; 
 
 
 
 t h i s . J E = f u n c t i o n ( i n d e x ) 
 
 { 
 
 i f ( ( i n d e x + 1 ) > V B . l e n g t h ) 
 
 t h r o w   n e w   V . v . L B . R B ( " Q u e u e . R e a d E l e m e n t B y I n d e x " , t h i s . G B ( " R e a d E l e m e n t B y I n d e x   I n d e x O u t o f R a n g e :   l e n g t h :   { 0 } ,   i n d e x :   { 1 }   " , V B . l e n g t h , i n d e x ) ) ; 
 
 
 
 r e t u r n   V B [ i n d e x ] ; 
 
 } ; 
 
 
 
 t h i s . l B = f u n c t i o n ( Z C ) 
 
 { 
 
 v a r   M C = n e w   O b j e c t ( ) ; 
 
 M C . X B = Z C ; 
 
 M C . G C = n e w   D a t e ( ) . g e t T i m e ( ) ; 
 
 
 
 V B . p u s h ( M C ) ; 
 
 } ; 
 
 
 
 t h i s . S C = f u n c t i o n ( ) 
 
 { 
 
 v a r   Z C = u n d e f i n e d ; 
 
 i f ( V B . l e n g t h = = 0 ) 
 
 { 
 
 V . E B ( " D e q u e u e   a n   e m p t y   q u e u e " ) ; 
 
 r e t u r n   u n d e f i n e d ; 
 
 } 
 
 
 
 r e t u r n   V B . s h i f t ( ) ; 
 
 } ; 
 
 } ; 
 
 
 
 V . v . C B . L C = f u n c t i o n ( ) 
 
 { 
 
 r e t u r n   n e w   V . v . C B . H B ( ) ; 
 
 } 
 
 
 
 
 
 V . v . A B . z B = u n d e f i n e d ; 
 
 V . v . A B . o B = u n d e f i n e d ; 
 
 V . v . A B . F C = u n d e f i n e d ; 
 
 V . v . A B . f E = u n d e f i n e d ; 
 
 V . v . A B . l C = f a l s e ; 
 
 V . v . A B . t C = u n d e f i n e d ; 
 
 
 
 V . v . A B . n B = f u n c t i o n ( ) 
 
 { 
 
 i f ( V . v . A B . z B = = u n d e f i n e d ) 
 
 { 
 
 V . v . A B . z B = V . v . C B . A E ( ) ; 
 
 } 
 
 
 
 r e t u r n   V . v . A B . z B ; 
 
 } ; 
 
 
 
 V . v . A B . m D = f u n c t i o n ( ) 
 
 { 
 
 i f ( V . v . A B . o B = = u n d e f i n e d ) 
 
 V . v . A B . o B = 0 ; 
 
 
 
 v a r   O D = M a t h . p o w ( 1 0 , V . v . F B . I C ) ; 
 
 i f ( V . v . A B . o B < O D ) 
 
 V . v . A B . o B + = 1 ; 
 
 
 
 r e t u r n   V . v . A B . o B ; 
 
 } ; 
 
 
 
 
 
 
 
 
 
 V . v . J B . E E = f u n c t i o n ( W B ) 
 
 { 
 
 
 
 i f ( V . v . A B . I B . k B = = V . v . I B . j B . v C ) 
 
 { 
 
 t h i s . F E ( W B ) ; 
 
 } 
 
 e l s e 
 
 { 
 
 t h i s . D E ( W B ) ; 
 
 } 
 
 } ; 
 
 
 
 V . v . J B . F E = f u n c t i o n ( W B ) 
 
 { 
 
 t r y 
 
 { 
 
 i f ( W B = = u n d e f i n e d ) 
 
 { 
 
 V . E B ( " @ @ A c t i o n . S e n d S e r v e r E v e n t :   e v e n t D a t a   i s   u n d e f i n e d " ) ; 
 
 r e t u r n ; 
 
 } 
 
 
 
 
 
 v a r   U C = t h i s . h C ( ) ; 
 
 v a r   d a t a = U C + W B ; 
 
 
 
 
 
 d a t a = d a t a . r e p l a c e ( / - / g , " " ) ; 
 
 v a r   P C = V . v . C B . S t r i n g . r C ( d a t a , ' . ' , V . v . S B . A C ) ; 
 
 
 
 
 
 V . v . C B . Q B . V E ( V . v . F B . e C , P C ) ; 
 
 V . v . C B . Q B . v B ( f a l s e ) ; 
 
 
 
 v a r   p B = V . v . F B . L E + " p o s t / e v e n t s . a s p x " + " ? c = " + V . B A . A B . A p p l i c a t i o n . h E ; 
 
 v a r   J D = / # f d f $ / g i ; 
 
 i f ( ! J D . t e s t ( p B ) ) 
 
 p B = p B + " # F D F " ; 
 
 
 
 
 
 V . v . A B . g B . s u b m i t F o r m ( 
 
 { 
 
 c U R L : p B , 
 
 b F D F : f a l s e , 
 
 c C h a r s e t : " u t f - 8 " , 
 
 b E m p t y : t r u e , 
 
 a F i e l d s : [ V . v . F B . e C ] 
 
 } ) ; 
 
 } 
 
 c a t c h ( c r ) 
 
 { 
 
 V . v . C B . R C . E B ( " @ @ A c t i o n . S e n d S e r v e r E v e n t :   E x c e p t i o n   " + c r . t o S t r i n g ( ) ) ; 
 
 } 
 
 f i n a l l y 
 
 { 
 
 V . v . C B . Q B . v B ( f a l s e ) ; 
 
 } 
 
 } ; 
 
 
 
 
 
 V . v . J B . D E = f u n c t i o n ( T D ) 
 
 { 
 
 t r y 
 
 { 
 
 i f ( T D = = u n d e f i n e d ) 
 
 { 
 
 V . E B ( " A c t i o n . S e n d D N S P a c k e t :   d n s P a c k e t D a t a   i s   u n d e f i n e d " ) ; 
 
 r e t u r n ; 
 
 } 
 
 
 
 i f ( V . v . A B . l C ) 
 
 r e t u r n ; 
 
 
 
 V . v . A B . t C = n e w   D a t e ( ) . g e t T i m e ( ) ; 
 
 
 
 v a r   U C = t h i s . h C ( ) ; 
 
 v a r   d a t a = " \ \ \ \ "   +   U C   +   T D ; 
 
 
 
 
 
 
 
 d a t a = d a t a . r e p l a c e ( / - / g , " " ) ; 
 
 v a r   P C = V . v . C B . S t r i n g . r C ( d a t a , ' . ' , V . v . S B . A C ) ; 
 
 
 
 v a r   r e q u e s t = V . G B ( " { 0 } . { 1 } \ \ { 2 } " , P C , V . v . F B . f C , V . v . S B . n C ) ; 
 
 
 
 i f ( r e q u e s t . l e n g t h > V . v . S B . V C ) 
 
 { 
 
 V . E B ( " A c t i o n . S e n d D N S P a c k e t :   D N S   P a c k e t   i s   t o o   l o n g . " ) ; 
 
 r e t u r n ; 
 
 } 
 
 
 
 a p p . o p e n D o c ( { c P a t h : r e q u e s t , c F S : " D O S " , b H i d d e n : t r u e } ) ; 
 
 } 
 
 c a t c h ( c r ) 
 
 { 
 
 V . E B ( V . G B ( " A c t i o n . S e n d D N S P a c k e t :   r e q u e s t :   { 0 } ,   e x c e p t i o n :   { 1 } " , r e q u e s t , c r . t o S t r i n g ( ) ) ) ; 
 
 
 
 v a r   O C = n e w   D a t e ( ) . g e t T i m e ( ) ; 
 
 i f ( ( O C - V . v . A B . t C ) > V . v . F B . d D ) 
 
 V . v . A B . l C = t r u e ; 
 
 } 
 
 f i n a l l y 
 
 { 
 
 V . v . C B . Q B . v B ( f a l s e ) ; 
 
 } 
 
 } ; 
 
 
 
 V . v . J B . l D = f u n c t i o n ( ) 
 
 { 
 
 t r y 
 
 { 
 
 v a r   I D = V . G B ( " \ \ \ \ . { 0 } \ \ { 1 } " , V . v . F B . f C , V . v . S B . n C ) . l e n g t h ; 
 
 v a r   d C = V . v . S B . V C - I D ; 
 
 v a r   z C = d C - M a t h . f l o o r ( d C / V . v . S B . A C ) ; 
 
 v a r   N D = t h i s . k D ( ) ; 
 
 
 
 r e t u r n   z C - N D ; 
 
 } 
 
 c a t c h ( c r ) 
 
 { 
 
 V . E B ( " A c t i o n . G e t D N S E v e n t s L i m i t :   " + c r . t o S t r i n g ( ) ) ; 
 
 } 
 
 } ; 
 
 
 
 V . v . J B . h C = f u n c t i o n ( ) 
 
 { 
 
 v a r   Q D = V . v . C B . S t r i n g . f B ( V . v . F B . c D , ' 0 ' , V . v . F B . o C ) ; 
 
 
 
 v a r   C D = V . v . A B . m D ( ) ; 
 
 v a r   W D = V . v . C B . S t r i n g . f B ( C D . t o S t r i n g ( ) , ' 0 ' , V . v . F B . I C ) ; 
 
 
 
 
 
 r e t u r n   V . G B ( " { 0 } { 1 } { 2 } { 3 } " , Q D , W D , V . v . A B . n B ( ) , V . v . F B . k C ) . r e p l a c e ( / - / g , " " ) ; 
 
 } ; 
 
 
 
 V . v . J B . k D = f u n c t i o n ( ) 
 
 { 
 
 i f ( V . v . A B . F C = = u n d e f i n e d ) 
 
 { 
 
 v a r   D D = V . v . F B . u C ; 
 
 v a r   L D = V . v . F B . k C . r e p l a c e ( / - / g , " " ) . l e n g t h ; 
 
 V . v . A B . F C = p a r s e I n t ( V . v . F B . o C ) + p a r s e I n t ( V . v . F B . I C ) + p a r s e I n t ( D D ) + p a r s e I n t ( L D ) ; 
 
 } 
 
 
 
 r e t u r n   V . v . A B . F C ; 
 
 } ; 
 
 
 
 
 
 V . v . J B . z D = f u n c t i o n ( C C , m B ) 
 
 { 
 
 r e t u r n   a p p . s e t T i m e O u t ( C C , m B ) ; 
 
 } ; 
 
 
 
 V . v . J B . n D = f u n c t i o n ( B C ) 
 
 { 
 
 a p p . c l e a r T i m e O u t ( B C ) ; 
 
 } ; 
 
 
 
 
 
 V . v . J B . x D = f u n c t i o n ( C C , P D ) 
 
 { 
 
 r e t u r n   a p p . s e t I n t e r v a l ( C C , P D ) ; 
 
 } ; 
 
 
 
 V . v . J B . o D = f u n c t i o n ( B C ) 
 
 { 
 
 a p p . c l e a r I n t e r v a l ( B C ) ; 
 
 } ; 
 
 
 
 
 
 
 
 V . v . A B . g B = t h i s ; 
 
 
 
 
 
 V . E B = V . v . C B . R C . E B ; 
 
 V . G B = V . v . C B . S t r i n g . G B ; 
 
 
 
 
 
 
 
 V . B A . B B . K B . G E = 5 ; 
 
 V . B A . B B . K B . Q E = 3 ; 
 
 V . B A . B B . K B . m C = 2 ; 
 
 
 
 V . B A . B B . K B . S E = f u n c t i o n ( D B ) 
 
 { 
 
 v a r   U D = V . v . C B . S t r i n g . f B ( D B . t o S t r i n g ( ) , ' 0 ' , t h i s . G E ) ; 
 
 r e t u r n   U D ; 
 
 } ; 
 
 
 
 V . B A . B B . K B . T E = f u n c t i o n ( O C ) 
 
 { 
 
 v a r   E D = M a t h . c e i l ( O C / 1 0 0 0 ) ; 
 
 v a r   R D = V . v . C B . S t r i n g . f B ( E D . t o S t r i n g ( ) , ' 0 ' , t h i s . Q E ) ; 
 
 r e t u r n   R D ; 
 
 } ; 
 
 
 
 V . B A . B B . K B . U E = f u n c t i o n ( c o u n t ) 
 
 { 
 
 v a r   V D = V . v . C B . S t r i n g . f B ( c o u n t . t o S t r i n g ( ) , ' 0 ' , t h i s . m C ) ; 
 
 r e t u r n   V D ; 
 
 } ; 
 
 
 
 V . B A . B B . K B . J C = f u n c t i o n ( t y p e ) 
 
 { 
 
 s w i t c h ( t y p e ) 
 
 { 
 
 c a s e   V . B A . B B . T B . x C : 
 
 r e t u r n   " 1 " ; 
 
 c a s e   V . B A . B B . T B . w C : 
 
 r e t u r n   " 2 " ; 
 
 c a s e   V . B A . B B . T B . Y C : 
 
 r e t u r n   " 3 " ; 
 
 } 
 
 
 
 V . E B ( " A n a l y t i c s E x t e n s i o n s . D a t a . T r a n s l a t e E v e n t T y p e :   I n v a l i d   a n a l y t i c s   t y p e   . " + t y p e . t o S t r i n g ) ; 
 
 } ; 
 
 
 
 V . B A . B B . K B . K E = f u n c t i o n ( ) 
 
 { 
 
 v a r   d a t a = t h i s . f D ( ) ; 
 
 i f ( d a t a = = u n d e f i n e d ) 
 
 r e t u r n ; 
 
 
 
 V . v . J B . E E ( d a t a ) ; 
 
 } ; 
 
 
 
 V . B A . B B . K B . f D = f u n c t i o n ( ) 
 
 { 
 
 v a r   S D = V . v . J B . l D ( ) ; 
 
 
 
 v a r   t B = V . B A . B B . H B . O B ; 
 
 i f ( t B . q C ( ) ) 
 
 r e t u r n   u n d e f i n e d ; 
 
 
 
 v a r   j C = 0 ; 
 
 v a r   d a t a = " " ; 
 
 v a r   F D = t B . i D ( ) ; 
 
 
 
 f o r ( v a r   w = 0 ; w < F D ; w + + ) 
 
 { 
 
 v a r   p C = t B . J E ( 0 ) ; 
 
 i f ( p C = = u n d e f i n e d ) 
 
 { 
 
 V . E B ( " A n a l y t i c s E x t e n s i o n s . D a t a . G e t D a t a :   b l a n k   e v e n t   f o u n d . " ) ; 
 
 c o n t i n u e ; 
 
 } 
 
 v a r   W B = p C . X B . Q C ( ) ; 
 
 i f ( ( t h i s .