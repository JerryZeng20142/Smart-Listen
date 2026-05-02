#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Media::Playback;
using namespace Windows::Media::Core;
using namespace Windows::Foundation;

namespace winrt::SmartListen::implementation
{
    MainWindow::MainWindow()
    {
        m_mediaPlayer = MediaPlayer();
        m_mediaPlayer.Volume(0.5);
        m_mediaPlayer.MediaEnded([this](MediaPlayer const&, IInspectable const&)
            {
                m_isPlaying = false;
                UpdatePlayPauseIcon();
            });

        InitializeComponent();
    }

    void MainWindow::OpenFileButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        OpenFileAsync();
    }

    winrt::Windows::Foundation::IAsyncAction MainWindow::OpenFileAsync()
    {
        auto lifetime = get_strong();

        auto picker = FileOpenPicker();
        picker.SuggestedStartLocation(PickerLocationId::MusicLibrary);
        picker.FileTypeFilter().Append(L".mp3");
        picker.FileTypeFilter().Append(L".wav");
        picker.FileTypeFilter().Append(L".m4a");
        picker.FileTypeFilter().Append(L".wma");
        picker.FileTypeFilter().Append(L".flac");
        picker.FileTypeFilter().Append(L".ogg");
        picker.FileTypeFilter().Append(L".aac");

        auto file = co_await picker.PickSingleFileAsync();
        if (file)
        {
            auto source = MediaSource::CreateFromStorageFile(file);
            m_mediaPlayer.Source(source);

            SongTitleText().Text(file.Name());

            m_isPlaying = false;
            UpdatePlayPauseIcon();

            co_await resume_after(std::chrono::milliseconds(200));

            if (m_mediaPlayer.PlaybackSession().NaturalDuration().count() > 0)
            {
                m_duration = m_mediaPlayer.PlaybackSession().NaturalDuration();
                TotalTimeText().Text(FormatTime(m_duration));
            }
        }
    }

    void MainWindow::PlayPauseButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (m_mediaPlayer.Source() == nullptr)
        {
            return;
        }

        if (m_isPlaying)
        {
            m_mediaPlayer.Pause();
        }
        else
        {
            m_mediaPlayer.Play();
            StartTimeUpdateTimer();
        }

        m_isPlaying = !m_isPlaying;
        UpdatePlayPauseIcon();
    }

    void MainWindow::PreviousButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        m_mediaPlayer.Position(TimeSpan{ 0 });
        UpdateTimeDisplay();
    }

    void MainWindow::ProgressSlider_ValueChanged(IInspectable const&, 
        Primitives::RangeBaseValueChangedEventArgs const& e)
    {
        if (m_isSliderChanging || m_duration.count() == 0)
        {
            return;
        }

        m_isSliderChanging = true;
        auto position = static_cast<int64_t>(e.NewValue() / 100.0 * m_duration.count());
        m_mediaPlayer.Position(TimeSpan{ position });
        
        CurrentTimeText().Text(FormatTime(m_mediaPlayer.Position()));
        
        m_isSliderChanging = false;
    }

    void MainWindow::VolumeSlider_ValueChanged(IInspectable const&, 
        Primitives::RangeBaseValueChangedEventArgs const& e)
    {
        m_mediaPlayer.Volume(e.NewValue() / 100.0);
    }

    void MainWindow::UpdatePlayPauseIcon()
    {
        PlayPauseIcon().Glyph(m_isPlaying ? L"\xE769" : L"\xE768");
    }

    void MainWindow::UpdateTimeDisplay()
    {
        if (m_duration.count() == 0)
        {
            if (m_mediaPlayer.PlaybackSession().NaturalDuration().count() > 0)
            {
                m_duration = m_mediaPlayer.PlaybackSession().NaturalDuration();
                TotalTimeText().Text(FormatTime(m_duration));
            }
        }

        if (!m_isSliderChanging)
        {
            auto position = m_mediaPlayer.Position();
            CurrentTimeText().Text(FormatTime(position));

            if (m_duration.count() > 0)
            {
                auto progress = (position.count() * 100.0) / m_duration.count();
                ProgressSlider().Value(progress);
            }
        }
    }

    hstring MainWindow::FormatTime(TimeSpan time)
    {
        auto totalSeconds = static_cast<int64_t>(time.count() / 10000000);
        auto minutes = totalSeconds / 60;
        auto seconds = totalSeconds % 60;

        wchar_t buffer[32];
        swprintf_s(buffer, L"%lld:%02lld", minutes, seconds);
        return buffer;
    }

    winrt::Windows::Foundation::IAsyncAction MainWindow::StartTimeUpdateTimer()
    {
        auto lifetime = get_strong();
        
        while (m_isPlaying)
        {
            co_await resume_after(std::chrono::seconds(1));
            
            if (m_mediaPlayer.PlaybackSession().PlaybackState() == MediaPlaybackState::Playing)
            {
                UpdateTimeDisplay();
            }
        }
    }
}
