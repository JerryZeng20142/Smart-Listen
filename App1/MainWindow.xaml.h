#pragma once

#include "MainWindow.g.h"

namespace winrt::App1::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        void OpenButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void PlayPauseButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void StopButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ProgressSlider_ValueChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e);
        void VolumeSlider_ValueChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e);

    private:
        winrt::Windows::Media::Playback::MediaPlayer m_mediaPlayer{ nullptr };
        winrt::Windows::Foundation::TimeSpan m_duration{ 0 };
        bool m_isPlaying{ false };
        bool m_isSliderChanging{ false };
        winrt::Windows::Foundation::IAsyncAction m_timer{ nullptr };
        HWND m_hwnd{ nullptr };

        winrt::Windows::Foundation::IAsyncAction OpenFileAsync();
        void UpdatePlayPauseButton();
        void UpdateTimeDisplay();
        winrt::hstring FormatTime(winrt::Windows::Foundation::TimeSpan time);
        void StartTimer();
        void StopTimer();
    };
}

namespace winrt::App1::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
