#include "pch.h"
#include "App.xaml.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::SmartListen::implementation
{
    App::App()
    {
    }

    void App::OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const&)
    {
        Window window = Window::Create();
        window.Content(winrt::make<MainWindow>());
        window.Activate();
    }
}
