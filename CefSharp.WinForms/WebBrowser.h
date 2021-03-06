#include "stdafx.h"
#pragma once

#include "CefSharp.h"
#include "ClientAdapter.h"
#include "IWebBrowser.h"
#include "ConsoleMessageEventArgs.h"
#include "RtzCountdownEvent.h"
#include "BrowserSettings.h"
#include "BrowserCore.h"
#include "ScriptCore.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Threading;

namespace CefSharp
{
    public ref class WebBrowser sealed : public Control, IWebBrowser
    {
        BrowserSettings^ _settings; // XXX: move to BrowserCore?

        ManualResetEvent^ _initialized;

        MCefRefPtr<ClientAdapter> _clientAdapter;
        BrowserCore^ _browserCore;
        MCefRefPtr<ScriptCore> _scriptCore;

    protected:
        virtual void OnHandleCreated(EventArgs^ e) override;
        virtual void OnSizeChanged(EventArgs^ e) override;
        virtual void OnGotFocus(EventArgs^ e) override;

    private:
        void Construct(String^ address, BrowserSettings^ settings)
        {
            _initialized = gcnew ManualResetEvent(false);

            _browserCore = gcnew BrowserCore();
            _scriptCore = new ScriptCore();

            _browserCore->Address = address;
            _settings = settings;

            if(!CEF::IsInitialized)
            {
                if(!CEF::Initialize(gcnew Settings()))
                {
                    throw gcnew InvalidOperationException("CEF initialization failed.");
                }
            }
        }

        void WaitForInitialized();

    public:
        virtual event PropertyChangedEventHandler^ PropertyChanged
        {
            void add(PropertyChangedEventHandler^ handler)
            {
                _browserCore->PropertyChanged += handler;
            }

            void remove(PropertyChangedEventHandler^ handler)
            {
                _browserCore->PropertyChanged -= handler;
            }
        }

        event ConsoleMessageEventHandler^ ConsoleMessage;

        WebBrowser()
        {
            Construct("about:blank", gcnew BrowserSettings);
        }

        WebBrowser(String^ initialUrl, BrowserSettings^ settings)
        {
            Construct(initialUrl, settings);
        }

        property bool IsInitialized
        {
            bool get()
            {
                return
                    _clientAdapter.get() != nullptr &&
                    _clientAdapter->GetIsInitialized();
            }
        }

        virtual property bool IsLoading
        {
            bool get() { return _browserCore->IsLoading; }
        }

        virtual property bool CanGoForward
        { 
            bool get() { return _browserCore->CanGoForward; } 
        }

        virtual property bool CanGoBack
        { 
            bool get() { return _browserCore->CanGoBack; } 
        }

        virtual property String^ Address
        {
            String^ get() { return _browserCore->Address; }
            void set(String^ address) { _browserCore->Address = address; }
        }

        virtual property String^ Title
        {
            String^ get() { return _browserCore->Title; }
            void set(String^ title) { _browserCore->Title = title; }
        }

        virtual property String^ Tooltip
        {
            String^ get() { return _browserCore->Tooltip; }
            void set(String^ tooltip) { _browserCore->Tooltip = tooltip; }
        }

        virtual property IBeforePopup^ BeforePopupHandler
        {
            IBeforePopup^ get() { return _browserCore->BeforePopupHandler; }
            void set(IBeforePopup^ handler) { _browserCore->BeforePopupHandler = handler; }
        }

        virtual property IBeforeResourceLoad^ BeforeResourceLoadHandler
        {
            IBeforeResourceLoad^ get() { return _browserCore->BeforeResourceLoadHandler; }
            void set(IBeforeResourceLoad^ handler) { _browserCore->BeforeResourceLoadHandler = handler; }
        }

        virtual property IBeforeMenu^ BeforeMenuHandler
        {
            IBeforeMenu^ get() { return _browserCore->BeforeMenuHandler; }
            void set(IBeforeMenu^ handler) { _browserCore->BeforeMenuHandler = handler; }
        }

        virtual property IAfterResponse^ AfterResponseHandler
        {
            IAfterResponse^ get() { return _browserCore->AfterResponseHandler; }
            void set(IAfterResponse^ handler) { _browserCore->AfterResponseHandler = handler; }
        }

        virtual void OnInitialized();

        void Load(String^ url);
        void Stop();
        void Back();
        void Forward();
        void Reload();
        void Reload(bool ignoreCache);
        void Print();

        String^ RunScript(String^ script);

        virtual void SetNavState(bool isLoading, bool canGoBack, bool canGoForward);

        virtual void AddFrame(CefRefPtr<CefFrame> frame);
        virtual void FrameLoadComplete(CefRefPtr<CefFrame> frame);

        virtual void RaiseConsoleMessage(String^ message, String^ source, int line);
    };
}
