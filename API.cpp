#ifndef emFpPlugin_h
#define emFpPlugin_h

#ifndef emPanel_h
#include <emCore/emPanel.h>
#endif

class emFpPlugin : public emStructRec {

public:

       
        emFpPlugin();
               

        virtual ~emFpPlugin();
                
        emTArrayRec<emStringRec> FileTypes;
                 
        emDoubleRec Priority;
                

        emStringRec Library;

        emStringRec Function;
                
        class PropertyRec : public emStructRec {
        public:
                PropertyRec();
                virtual ~PropertyRec();
                emStringRec Name;
                emStringRec Value;
        };
        emTArrayRec<PropertyRec> Properties;


        PropertyRec * GetProperty(const char * name);

        emPanel * TryCreateFilePanel(
                emPanel::ParentArg parent, const emString & name,
                const emString & path
        );

        virtual const char * GetFormatName() const;

private:
        void * CachedFunc;
        emString CachedFuncLib;
        emString CachedFuncName;
};

extern "C" {
        typedef emPanel * (*emFpPluginFunc) (
                emPanel::ParentArg parent, const emString & name,
                const emString & path, emFpPlugin * plugin,
                emString * errorBuf
        );
}

class emFpPluginList : public emModel {

public:


        static emRef<emFpPluginList> Acquire(emRootContext & rootContext);

        emPanel * CreateFilePanel(
                emPanel::ParentArg parent, const emString & name,
                const emString & path, int alternative=0
        );
                
        emPanel * CreateFilePanel(
                emPanel::ParentArg parent, const emString & name,
                const emString & absolutePath, int statErr, long statMode,
                int alternative=0
        );

protected:

        emFpPluginList(emContext & context, const emString & name);
        virtual ~emFpPluginList();

private:

        static int CmpReversePluginPriorities(
                emFpPlugin * const * obj1, emFpPlugin * const * obj2,
                void * context
        );

        emArray<emFpPlugin*> Plugins;
             
};


#ifndef emFilePanel_h
#define emFilePanel_h

#ifndef emPanel_h
#include <emCore/emPanel.h>
#endif

#ifndef emFileModel_h
#include <emCore/emFileModel.h>
#endif


//==============================================================================
//================================ emFilePanel =================================
//==============================================================================

class emFilePanel : public emPanel {

public:

       
        emFilePanel(
                ParentArg parent, const emString & name,
                emFileModel * fileModel=NULL, bool updateFileModel=true
        );
                

        virtual ~emFilePanel();
                // Destructor.

        emFileModel * GetFileModel() const;
                // Get the file model, NULL if none.

        virtual void SetFileModel(emFileModel * fileModel,
                                  bool updateFileModel=true);
                // Set the file model, NULL for none. If updateFileModel==true,
                // fileModel->Update() is called.

        void SetCustomError(const emString & message);
        void ClearCustomError();
        emString GetCustomError() const;
                // Set, clear or get a custom error message. If set, the message
                // is shown by the default implementation of Paint.

        const emSignal & GetVirFileStateSignal() const;
                // Signaled when the virtual file state has changed.

        enum VirtualFileState {
                VFS_WAITING       = emFileModel::FS_WAITING,
                VFS_LOADING       = emFileModel::FS_LOADING,
                VFS_LOADED        = emFileModel::FS_LOADED,
                VFS_UNSAVED       = emFileModel::FS_UNSAVED,
                VFS_SAVING        = emFileModel::FS_SAVING,
                VFS_TOO_COSTLY    = emFileModel::FS_TOO_COSTLY,
                VFS_LOAD_ERROR    = emFileModel::FS_LOAD_ERROR,
                VFS_SAVE_ERROR    = emFileModel::FS_SAVE_ERROR,
                VFS_NO_FILE_MODEL = emFileModel::FS_MAX_VAL+1,
                VFS_CUSTOM_ERROR  = emFileModel::FS_MAX_VAL+2
        };
        VirtualFileState GetVirFileState() const;
               
        bool IsVFSGood() const;
                // This is a short-cut for:
                //  (GetVirFileState()==VFS_LOADED ||
                //   GetVirFileState()==VFS_UNSAVED)
                // It means that the file model data can safely be shown and
                // modified.

        virtual emString GetIconFileName() const;

        virtual bool IsContentReady(bool * pReadying=NULL) const;

protected:

        virtual bool Cycle();

        virtual void Notice(NoticeFlags flags);

        virtual bool IsOpaque() const;

        virtual void Paint(const emPainter & painter, emColor canvasColor) const;
                // Paints some info about the virtual file state including any
                // error messages. Should be overloaded for showing the file
                // model data when IsVFSGood()==true. InvalidatePainting is
                // called automatically on each change of the virtual file
                // state.

        virtual bool IsHopeForSeeking() const;
                // Returns true if VFS_WAITING, VFS_LOADING or VFS_SAVING.

private:

        emFileModelClient FileModelClient;
        emString * CustomError;
        emSignal VirFileStateSignal;
};

inline emFileModel * emFilePanel::GetFileModel() const
{
        return FileModelClient.GetModel();
}

inline const emSignal & emFilePanel::GetVirFileStateSignal() const
{
        return VirFileStateSignal;
}

#ifndef emRecFileModel_h
#define emRecFileModel_h

#ifndef emRec_h
#include <emCore/emRec.h>
#endif

#ifndef emFileModel_h
#include <emCore/emFileModel.h>
#endif

class emRecFileModel : public emFileModel {

public:

        

        const emSignal & GetChangeSignal() const;
                // Signaled on every modification of the record.

protected:

        emRecFileModel(emContext & context, const emString & name);
                // Do not forget to call PostConstruct from an overloaded
                // constructor.

        void PostConstruct(emRec & rec);
                // Must be called by the constructor of the derived class after
                // the record has been constructed.

        virtual ~emRecFileModel();
                // Destructor.

        emRec & GetRec();
                // Not valid before PostConstruct has been called.

        virtual void ResetData();
        virtual void TryStartLoading();
        virtual bool TryContinueLoading();
        virtual void QuitLoading();
        virtual void TryStartSaving();
        virtual bool TryContinueSaving();
        virtual void QuitSaving();
        virtual emUInt64 CalcMemoryNeed();
        virtual double CalcFileProgress();

private:

        class RecLink : public emRecListener {
        public:
                RecLink(emRecFileModel & model);
        protected:
                virtual void OnRecChanged();
        private:
                emRecFileModel & Model;
        };
        friend class RecLink;

        emSignal ChangeSignal;
        RecLink Link;
        emRecFileReader * Reader;
        emRecFileWriter * Writer;
        int ProtectFileState;
        emUInt64 MemoryNeed;
        int MemoryNeedOutOfDate;
        int ReadStep, ReadStepOfMemCalc;
};

inline const emSignal & emRecFileModel::GetChangeSignal() const
{
        return ChangeSignal;
}

inline emRec & emRecFileModel::GetRec()
{
        return *Link.GetListenedRec();
}


#include <emCore/emRecFileModel.h>
#include <emCore/emFilePanel.h>
#include <emCore/emToolkit.h>
#include <emCore/emFpPlugin.h>



class MyPanel : public emPanel {
public:
        MyPanel(ParentArg parent, const emString & name);
        virtual emString GetTitle() const;
protected:
        virtual void Paint(const emPainter & painter, emColor canvasColor) const;
};

MyPanel::MyPanel(ParentArg parent, const emString & name)
        : emPanel(parent,name)
{
}

emString MyPanel::GetTitle() const
{
        return "Hello World Example";
}

void MyPanel::Paint(const emPainter & painter, emColor canvasColor) const
{
        painter.Clear(emColor::BLACK);
        painter.PaintTextBoxed(0,0,1,GetHeight(),"Hello World!",.01,emColor::RED);
}


MAIN_OR_WINMAIN_HERE

static int wrapped_main(int argc, char * argv[])
{
        emInitLocale();

        emGUIFramework framework;
        framework.EnableAutoTermination();

        emWindow * window=new emWindow(framework.GetRootContext());
        window->SetWindowFlags(emWindow::WF_AUTO_DELETE);

        emPanel * panel=new MyPanel(window,"root");
        panel->Layout(0.0,0.0,4.0,3.0);

        return framework.Run();
}

class MyPanel : public emPanel {
public:
        MyPanel(ParentArg parent, const emString & name);
protected:
        virtual bool IsOpaque() const;
        virtual void Paint(const emPainter & painter, emColor canvasColor) const;
private:
        emImage EagleIcon;
};

MyPanel::MyPanel(ParentArg parent, const emString & name)
        : emPanel(parent,name)
{
        EagleIcon=emGetInsResImage(GetRootContext(),"icons","eaglemode.tga");
}

bool MyPanel::IsOpaque() const
{
        return true;
}

void MyPanel::Paint(const emPainter & painter, emColor canvasColor) const
{
        painter.Clear(emColor::WHITE,canvasColor);
        canvasColor=emColor::WHITE;

        painter.PaintImage(0.05,0.05,0.3,0.3,EagleIcon,255,canvasColor);

        painter.PaintRect(0.4,0.1,0.2,0.2,emColor::GREEN,canvasColor);

        painter.PaintRectOutline(0.69,0.09,0.22,0.22,0.01,0x0080C0FF,canvasColor);
        painter.PaintTextBoxed(
                0.7,0.1,0.2,0.2,
                "Centered text\nin\nthe bottom-right\nof a box",
                0.017,0x0080C0FF,canvasColor,
                EM_ALIGN_BOTTOM_RIGHT,EM_ALIGN_CENTER
        );

        static const double poly1[]={
                0.1, 0.4,
                0.05, 0.7,
                0.3, 0.5
        };
        painter.PaintPolygon(poly1,3,emColor(255,128,0),canvasColor);

        painter.PaintLine(
                0.3,0.4,0.5,0.7,0.05,
                emPainter::LC_ROUND,emPainter::LC_ROUND,
                emColor(255,0,0,128),canvasColor
        );

        painter.PaintEllipse(0.5,0.4,0.3,0.3,0x33CC88FF,canvasColor);
        painter.PaintEllipse(0.6,0.4,0.3,0.3,0xFF55AAFF);
        painter.PaintEllipse(0.55,0.35,0.3,0.3,0xFFFF3388);
}

class MyPanel : public emPanel {
public:
        MyPanel(ParentArg parent, const emString & name);
protected:
        virtual void Input(emInputEvent & event, const emInputState & state,
                           double mx, double my);
private:
        bool XKeyDown;
        bool ButtonDown;
        double LastMX, LastMY;
};

MyPanel::MyPanel(ParentArg parent, const emString & name)
        : emPanel(parent,name)
{
        XKeyDown=false;
        ButtonDown=false;
}

void MyPanel::Input(
        emInputEvent & event, const emInputState & state, double mx, double my
)
{
        if (event.IsKey(EM_KEY_E) && state.IsNoMod()) {
                emLog("The E key was pressed without any modifier keys.");
                event.Eat();
        }

        if (event.IsKey(EM_KEY_G) && state.IsShiftAltMod()) {
                emLog("The Shift+Alt+G key combination was pressed.");
                event.Eat();
        }

        emInputHotkey hotkey(EM_KEY_CTRL, EM_KEY_V);
        if (hotkey.Match(event,state)) {
                emLog("The %s key combination was pressed.", hotkey.GetString().Get());
                event.Eat();
        }

        if (event.GetChars()=="$") {
                emLog("A key combination that prints the dollar sign was pressed.");
                event.Eat();
        }

        if (event.IsKey(EM_KEY_X)) {
                emLog("The X key was pressed.");
                XKeyDown=true;
                event.Eat();
        }
        if (XKeyDown && !state.Get(EM_KEY_X)) {
                emLog("The X key was released.");
                XKeyDown=false;
        }

        if (event.IsKey(EM_KEY_LEFT_BUTTON)) {
                emLog("The left mouse button was pressed at (%g, %g).", mx, my);
                ButtonDown=true;
                LastMX=mx;
                LastMY=my;
                // We do not eat the event here so that emPanel::Input still
                // sets the focus on this panel by the button event. Otherwise
                // we should call Focus().
        }
        if (ButtonDown && (LastMX!=mx || LastMY!=mx)) {
                emLog("The mouse was dragged to (%g, %g).", mx, my);
                LastMX=mx;
                LastMY=my;
        }
        if (ButtonDown && !state.Get(EM_KEY_LEFT_BUTTON)) {
                emLog("The left mouse button was released.");
                ButtonDown=false;
        }

        // Call the base class. It changes the focus by certain input events.
        emPanel::Input(event,state,mx,my);
}

class MyPanel : public emPanel {
public:
        MyPanel(ParentArg parent, const emString & name);
protected:
        virtual bool Cycle();
        virtual void Paint(const emPainter & painter, emColor canvasColor) const;
private:
        unsigned Counter;
};

MyPanel::MyPanel(ParentArg parent, const emString & name)
        : emPanel(parent,name)
{
        Counter=0;
        WakeUp();
}

bool MyPanel::Cycle()
{
        emPanel::Cycle();
        Counter++;
        InvalidatePainting();
        return true;
}

void MyPanel::Paint(const emPainter & painter, emColor canvasColor) const
{
        emString str=emString::Format("%u",Counter);
        painter.PaintTextBoxed(0,0,1,GetHeight(),str,.41,0xFFFF80FF);
}

class MyButton : public emPanel {
public:
        MyButton(ParentArg parent, const emString & name);
        const emSignal & GetClickSignal() const;
protected:
        virtual void Input(emInputEvent & event, const emInputState & state,
                           double mx, double my);
        virtual void Paint(const emPainter & painter, emColor canvasColor) const;
private:
        emSignal ClickSignal;
};

MyButton::MyButton(ParentArg parent, const emString & name)
        : emPanel(parent,name)
{
}

const emSignal & MyButton::GetClickSignal() const
{
        return ClickSignal;
}

void MyButton::Input(
        emInputEvent & event, const emInputState & state, double mx, double my
)
{
        if (event.IsLeftButton()) Signal(ClickSignal);
        emPanel::Input(event,state,mx,my);
}

void MyButton::Paint(const emPainter & painter, emColor canvasColor) const
{
        painter.Clear(0xC0C0C0FF,canvasColor);
        painter.PaintTextBoxed(0,0,1,GetHeight(),"Click Me",1,emColor::GREEN);
}


class MyPanel : public emPanel {
public:
        MyPanel(ParentArg parent, const emString & name);
protected:
        virtual bool Cycle();
        virtual void Paint(const emPainter & painter, emColor canvasColor) const;
private:
        unsigned Counter1, Counter2;
        MyButton * Button;
        emTimer Timer;
};

MyPanel::MyPanel(ParentArg parent, const emString & name)
        : emPanel(parent,name),
        Timer(GetScheduler())
{
        Counter1=0;
        Counter2=0;
        Button=new MyButton(this,"button");
        Button->Layout(0.1,0.1,0.8,0.1);
        Timer.Start(1000,true);
        AddWakeUpSignal(Button->GetClickSignal());
        AddWakeUpSignal(Timer.GetSignal());
}

bool MyPanel::Cycle()
{
        if (IsSignaled(Button->GetClickSignal())) {
                Counter1++;
                InvalidatePainting();
        }
        if (IsSignaled(Timer.GetSignal())) {
                Counter2++;
                InvalidatePainting();
        }
        return emPanel::Cycle();
}

void MyPanel::Paint(const emPainter & painter, emColor canvasColor) const
{
        emString str=emString::Format(
                "Button Signals: %u\nTimer Signals: %u",Counter1,Counter2
        );
        painter.PaintTextBoxed(0,0.3,1,GetHeight()-0.3,str,.1,0xFFFF80FF);
}

class MyPanel : public emPanel {
public:
        MyPanel(ParentArg parent, const emString & name, emColor bgcolor=0xFFFFFFFF);
protected:
        virtual bool IsOpaque() const;
        virtual void Paint(const emPainter & painter, emColor canvasColor) const;
        virtual void AutoExpand();
        virtual void LayoutChildren();
private:
        emColor BackgroundColor;
        emCrossPtr<MyPanel> Child[4];
};

MyPanel::MyPanel(ParentArg parent, const emString & name, emColor bgcolor)
        : emPanel(parent,name)
{
        BackgroundColor=bgcolor;
}

bool MyPanel::IsOpaque() const
{
        return BackgroundColor.IsOpaque();
}

void MyPanel::Paint(const emPainter & painter, emColor canvasColor) const
{
        painter.Clear(BackgroundColor,canvasColor);
}

void MyPanel::AutoExpand()
{
        for (int i=0; i<4; i++) {
                Child[i]=new MyPanel(
                        this,
                        emString::Format("%d",i),
                        BackgroundColor.Get()^0xFFFFFF00
                );
        }
}

void MyPanel::LayoutChildren()
{
        for (int i=0; i<4; i++) {
                if (Child[i]) {
                        Child[i]->Layout(
                                0.1+(i&1)*0.5,
                                (0.1+(i&2)*0.25)*GetHeight(),
                                0.3,
                                0.3*GetHeight(),
                                BackgroundColor
                        );
                }
        }
}


class MyPanel : public emRasterGroup {
public:
        MyPanel(ParentArg parent, const emString & name);
protected:
        virtual bool Cycle();
private:
        static void ScalarField2_TextOfValue(
                char * buf, int bufSize, emInt64 value, emUInt64 markInterval,
                void * context
        );
        emTextField * Message;
        emButton * Button;
        emCheckButton * CheckButton;
        emRadioButton::RasterGroup * RadioGroup;
        emTextField * TextField;
        emScalarField * ScalarField1;
        emScalarField * ScalarField2;
};

MyPanel::MyPanel(ParentArg parent, const emString & name)
        : emRasterGroup(parent,name,"Toolkit Example")
{
        Message=new emTextField(this,"msg","Message");
        Message->SetDescription("This text field shows messages about your input.");

        Button=new emButton(this,"b","Button");
        AddWakeUpSignal(Button->GetClickSignal());

        CheckButton=new emCheckButton(this,"cb","Check Button");
        AddWakeUpSignal(CheckButton->GetCheckSignal());

        RadioGroup=new emRadioButton::RasterGroup(this,"rg","Radio Group");
        RadioGroup->SetBorderScaling(4.0);
        new emRadioBox(RadioGroup,"rb1","Radio Box 1");
        new emRadioBox(RadioGroup,"rb2","Radio Box 2");
        new emRadioBox(RadioGroup,"rb3","Radio Box 3");
        AddWakeUpSignal(RadioGroup->GetCheckSignal());

        TextField=new emTextField(this,"tf","Text Field");
        TextField->SetEditable();
        TextField->SetMultiLineMode();
        AddWakeUpSignal(TextField->GetTextSignal());

        ScalarField1=new emScalarField(this,"sf1","Scalar Field 1");
        ScalarField1->SetEditable();
        ScalarField1->SetMinMaxValues(0,100);
        ScalarField1->SetScaleMarkIntervals(50,10,5,1,0);
        AddWakeUpSignal(ScalarField1->GetValueSignal());

        ScalarField2=new emScalarField(this,"sf2","Scalar Field 2");
        ScalarField2->SetEditable();
        ScalarField2->SetMinMaxValues(-1,1);
        ScalarField2->SetTextOfValueFunc(ScalarField2_TextOfValue);
        AddWakeUpSignal(ScalarField2->GetValueSignal());
}

bool MyPanel::Cycle()
{
        if (IsSignaled(Button->GetClickSignal())) {
                Message->SetText("Button clicked");
        }
        if (IsSignaled(CheckButton->GetCheckSignal())) {
                Message->SetText(emString::Format(
                        "Check Button switched %s",
                        CheckButton->IsChecked() ? "on" : "off"
                ));
        }
        if (IsSignaled(RadioGroup->GetCheckSignal())) {
                Message->SetText(emString::Format(
                        "Radio Box %d set",
                        RadioGroup->GetCheckIndex() + 1
                ));
        }
        if (IsSignaled(TextField->GetTextSignal())) {
                Message->SetText(emString::Format(
                        "Text Field changed to \"%s\"",
                        TextField->GetText().Get()
                ));
        }
        if (IsSignaled(ScalarField1->GetValueSignal())) {
                Message->SetText(emString::Format(
                        "Scalar Field 1 changed to %d",
                        (int)ScalarField1->GetValue()
                ));
        }
        if (IsSignaled(ScalarField2->GetValueSignal())) {
                Message->SetText(emString::Format(
                        "Scalar Field 2 changed to %d",
                        (int)ScalarField2->GetValue()
                ));
        }
        return emRasterGroup::Cycle();
}

void MyPanel::ScalarField2_TextOfValue(
        char * buf, int bufSize, emInt64 value, emUInt64 markInterval, void * context
)
{
        if (value<0) strcpy(buf,"left");
        else if (value==0) strcpy(buf,"middle");
        else strcpy(buf,"right");
}


//PlExFileModel

class PlExFileModel : public emRecFileModel, public emStructRec
{
public:

        static emRef<PlExFileModel> Acquire(
                emContext & context, const emString & name, bool common=true
        );

        virtual const char * GetFormatName() const;

        class LineRec : public emStructRec {
        public:
                LineRec();
                emDoubleRec X1,Y1,X2,Y2;
                emDoubleRec Thickness;
                emColorRec Color;
        };

        emColorRec CurrentColor;
        emTArrayRec<LineRec> Lines;

protected:

        PlExFileModel(emContext & context, const emString & name);
};


emRef<PlExFileModel> PlExFileModel::Acquire(
        emContext & context, const emString & name, bool common
)
{
        EM_IMPL_ACQUIRE(PlExFileModel,context,name,common)
}

const char * PlExFileModel::GetFormatName() const
{
        return "PlEx";
}

PlExFileModel::LineRec::LineRec()
        : emStructRec(),
        X1(this,"X1"),
        Y1(this,"Y1"),
        X2(this,"X2"),
        Y2(this,"Y2"),
        Thickness(this,"Thickness"),
        Color(this,"Color")
{
}

PlExFileModel::PlExFileModel(emContext & context, const emString & name)
        : emRecFileModel(context,name), emStructRec(),
        CurrentColor(this,"CurrentColor"),
        Lines(this,"Lines")
{
        PostConstruct(*this);
}


//PlExControlPanel

class PlExControlPanel : public emRasterGroup {
public:
        PlExControlPanel(ParentArg parent, const emString & name,
                         emRef<PlExFileModel> model);
protected:
        virtual bool Cycle();
private:
        emRef<PlExFileModel> Model;
        emColorField * ColorField;
        emButton * ClearButton;
};

PlExControlPanel::PlExControlPanel(
        ParentArg parent, const emString & name, emRef<PlExFileModel> model
)
        : emRasterGroup(parent,name,"PlEx")
{
        Model=model;
        SetPrefChildTallness(0.3);
        new emLabel(
                new emLinearGroup(this,"about","About"),
                "about",
                "This is PlEx - a plugin example.\n"
                "It allows to draw lines with the\n"
                "left mouse button.\n"
        );
        ColorField=new emColorField(this,"color","Color");
        ColorField->SetColor(Model->CurrentColor);
        ColorField->SetEditable();
        ClearButton=new emButton(
                new emTunnel(this,"tunnel","Clear"),
                "clear","Clear"
        );
        AddWakeUpSignal(ColorField->GetColorSignal());
        AddWakeUpSignal(ClearButton->GetClickSignal());
        AddWakeUpSignal(Model->GetChangeSignal());
}

bool PlExControlPanel::Cycle()
{
        if (IsSignaled(ColorField->GetColorSignal())) {
                Model->CurrentColor.Set(ColorField->GetColor());
                Model->Save(true);
        }
        if (IsSignaled(ClearButton->GetClickSignal())) {
                Model->Lines.SetCount(0);
                Model->Save(true);
        }
        if (IsSignaled(Model->GetChangeSignal())) {
                ColorField->SetColor(Model->CurrentColor);
        }
        return emRasterGroup::Cycle();
}


//PlExFilePanel

class PlExFilePanel : public emFilePanel {
public:
        PlExFilePanel(ParentArg parent, const emString & name,
                      emRef<PlExFileModel> model);
protected:
        virtual bool Cycle();
        virtual void Input(emInputEvent & event, const emInputState & state,
                           double mx, double my);
        virtual bool IsOpaque() const;
        virtual void Paint(const emPainter & painter, emColor canvasColor) const;
        virtual emPanel * CreateControlPanel(ParentArg parent,
                                             const emString & name);
private:
        emRef<PlExFileModel> Model;
        bool Drawing;
        double X1,Y1,X2,Y2;
        double Thickness;
        bool ControlPanelShown;
};

PlExFilePanel::PlExFilePanel(
        ParentArg parent, const emString & name,
        emRef<PlExFileModel> model
)
        : emFilePanel(parent,name,model)
{
        Model=model;
        Drawing=false;
        ControlPanelShown=false;
        AddWakeUpSignal(GetVirFileStateSignal());
        AddWakeUpSignal(Model->GetChangeSignal());
}

bool PlExFilePanel::Cycle()
{
        if (IsSignaled(GetVirFileStateSignal())) {
                if (ControlPanelShown!=IsVFSGood()) {
                        ControlPanelShown=IsVFSGood();
                        InvalidateControlPanel();
                }
        }
        if (IsSignaled(Model->GetChangeSignal())) {
                InvalidatePainting();
        }
        return emFilePanel::Cycle();
}

void PlExFilePanel::Input(
        emInputEvent & event, const emInputState & state, double mx, double my
)
{
        if (IsVFSGood()) {
                if (event.IsKey(EM_KEY_LEFT_BUTTON)) {
                        Drawing=true;
                        X1=X2=mx;
                        Y1=Y2=my;
                        Thickness=5.0/(GetViewedWidth()+GetViewedHeight());
                        InvalidatePainting();
                }
                if (Drawing && (X2!=mx || Y2!=mx)) {
                        X2=mx;
                        Y2=my;
                        InvalidatePainting();
                }
                if (Drawing && !state.Get(EM_KEY_LEFT_BUTTON)) {
                        Drawing=false;
                        int i=Model->Lines.GetCount();
                        Model->Lines.SetCount(i+1);
                        Model->Lines[i].X1=X1;
                        Model->Lines[i].Y1=Y1;
                        Model->Lines[i].X2=X2;
                        Model->Lines[i].Y2=Y2;
                        Model->Lines[i].Thickness=Thickness;
                        Model->Lines[i].Color=Model->CurrentColor.Get();
                        Model->Save(true);
                        InvalidatePainting();
                }
        }
        emFilePanel::Input(event,state,mx,my);
}

bool PlExFilePanel::IsOpaque() const
{
        if (IsVFSGood()) return true;
        else return emFilePanel::IsOpaque();
}

void PlExFilePanel::Paint(const emPainter & painter, emColor canvasColor) const
{
        if (IsVFSGood()) {
                painter.Clear(emColor::WHITE,canvasColor);
                for (int i=0; i<Model->Lines.GetCount(); i++) {
                        painter.PaintLine(
                                Model->Lines[i].X1,
                                Model->Lines[i].Y1,
                                Model->Lines[i].X2,
                                Model->Lines[i].Y2,
                                Model->Lines[i].Thickness,
                                emPainter::LC_ROUND,emPainter::LC_ROUND,
                                Model->Lines[i].Color
                        );
                }
                if (Drawing) {
                        painter.PaintLine(
                                X1,Y1,X2,Y2,Thickness,
                                emPainter::LC_ROUND,emPainter::LC_ROUND,
                                Model->CurrentColor
                        );
                }
        }
        else {
                emFilePanel::Paint(painter,canvasColor);
        }
}

emPanel * PlExFilePanel::CreateControlPanel(
        ParentArg parent, const emString & name
)
{
        if (IsVFSGood()) {
                return new PlExControlPanel(parent,name,Model);
        }
        else {
                return emFilePanel::CreateControlPanel(parent,name);
        }
}


//PlExFpPluginFunc

extern "C" {
        emPanel * PlExFpPluginFunc(
                emPanel::ParentArg parent, const emString & name,
                const emString & path, emFpPlugin * plugin,
                emString * errorBuf
        )
        {
                return new PlExFilePanel(
                        parent,
                        name,
                        PlExFileModel::Acquire(parent.GetRootContext(),path)
                );
        }
}