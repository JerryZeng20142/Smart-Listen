#pragma once

#include "App.g.h"

namespace winrt::SmartListen::implementation
{
    struct App : AppT<App>
    {
        App();
        void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const&);
    };
}
