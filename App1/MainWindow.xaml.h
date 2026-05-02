#pragma once

#include "MainWindow.g.h"

namespace winrt::SmartListen::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        void OpenFileButton_Click(winrt::Windows::Foundation::IInspectable const& sender, 
                                 winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void PlayPauseButton_Click(winrt::Windows::Foundation::IInspectable const& sender, 
                                   winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void PreviousButton_Click(winrt::Windows::Foundation::IInspectable const& sender, 
                                  winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ProgressSlider_ValueChanged(winrt::Windows::Foundation::IInspectable const& sender, 
                                        winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e);
        void VolumeSlider_ValueChanged(winrt::Windows::Foundation::IInspectable const& sender, 
                                      winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e);

    private:
        winrt::Windows::Media::Playback::MediaPlayer m_mediaPlayer{ nullptr };
        winrt::Windows::Foundation::TimeSpan m_duration{ 0 };
        bool m_isPlaying{ false };
        bool m_isSliderChanging{ false };

        winrt::Windows::Foundation::IAsyncAction OpenFileAsync();
        void UpdateTimeDisplay();
        winrt::hstring FormatTime(winrt::Windows::Foundation::TimeSpan time);
    };
}

namespace winrt::SmartListen::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
