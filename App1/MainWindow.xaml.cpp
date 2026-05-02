#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Windows.Media.Core.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Dispatching;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Media::Playback;
using namespace Windows::Media::Core;
using namespace Windows::Foundation;

namespace winrt::App1::implementation
{
    MainWindow::MainWindow()
    {
        m_mediaPlayer = MediaPlayer();
        m_mediaPlayer.Volume(0.5);
        m_mediaPlayer.MediaEnded([this](MediaPlayer const&, IInspectable const&)
            {
                StopTimer();
                m_isPlaying = false;
                UpdatePlayPauseButton();
            });

        InitializeComponent();

        m_dispatcher = DispatcherQueue::GetForCurrentThread();
    }

    void MainWindow::OpenButton_Click(IInspectable const&, RoutedEventArgs const&)
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

        auto hwnd = ::GetActiveWindow();
        auto initWindow = picker.try_as<IInitializeWithWindow>();
        if (initWindow && hwnd)
        {
            initWindow->Initialize(hwnd);
        }

        auto file = co_await picker.PickSingleFileAsync();
        if (file)
        {
            if (m_mediaPlayer.PlaybackSession().PlaybackState() != MediaPlaybackState::None)
            {
                StopButton_Click(nullptr, nullptr);
            }

            auto source = MediaSource::CreateFromStorageFile(file);
            m_mediaPlayer.Source(source);

            FileNameText().Text(file.Name());
            InfoBar().IsOpen(false);

            co_await winrt::resume_after(std::chrono::milliseconds(100));

            if (m_mediaPlayer.PlaybackSession().NaturalDuration().count() > 0)
            {
                m_duration = m_mediaPlayer.PlaybackSession().NaturalDuration();
                DurationText().Text(L"时长: " + FormatTime(m_duration));
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
            StopTimer();
        }
        else
        {
            m_mediaPlayer.Play();
            StartTimer();
        }

        m_isPlaying = !m_isPlaying;
        UpdatePlayPauseButton();
    }

    void MainWindow::StopButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        m_mediaPlayer.Pause();
        m_mediaPlayer.Position(TimeSpan{ 0 });
        m_isPlaying = false;
        StopTimer();
        UpdatePlayPauseButton();
        UpdateTimeDisplay();
    }

    void MainWindow::ProgressSlider_ValueChanged(IInspectable const&, Controls::Primitives::RangeBaseValueChangedEventArgs const& e)
    {
        if (m_isSliderChanging || m_duration.count() == 0)
        {
            return;
        }

        m_isSliderChanging = true;
        auto position = static_cast<int64_t>(e.NewValue() / 100.0 * m_duration.count());
        m_mediaPlayer.Position(TimeSpan{ position });
        m_isSliderChanging = false;
    }

    void MainWindow::VolumeSlider_ValueChanged(IInspectable const&, Controls::Primitives::RangeBaseValueChangedEventArgs const& e)
    {
        m_mediaPlayer.Volume(static_cast<double>(e.NewValue()) / 100.0);
    }

    void MainWindow::UpdatePlayPauseButton()
    {
        if (!m_dispatcher)
        {
            return;
        }

        m_dispatcher.TryEnqueue([this]()
        {
            auto button = PlayPauseButton().try_as<Controls::Button>();
            if (button)
            {
                auto icon = button.Content().try_as<Controls::FontIcon>();
                if (icon)
                {
                    icon.Glyph(m_isPlaying ? L"\xE769" : L"\xE768");
                }
            }
        });
    }

    void MainWindow::UpdateTimeDisplay()
    {
        if (!m_dispatcher)
        {
            return;
        }

        m_dispatcher.TryEnqueue([this]()
        {
            if (m_duration.count() == 0)
            {
                if (m_mediaPlayer.PlaybackSession().NaturalDuration().count() > 0)
                {
                    m_duration = m_mediaPlayer.PlaybackSession().NaturalDuration();
                    DurationText().Text(L"时长: " + FormatTime(m_duration));
                    TotalTimeText().Text(FormatTime(m_duration));
                }
            }

            auto position = m_mediaPlayer.Position();
            CurrentTimeText().Text(FormatTime(position));

            if (m_duration.count() > 0 && !m_isSliderChanging)
            {
                auto progress = (position.count() * 100.0) / m_duration.count();
                ProgressSlider().Value(progress);
            }
        });
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

    void MainWindow::StartTimer()
    {
        StopTimer();

        m_timer = [this]() -> IAsyncAction
        {
            auto lifetime = get_strong();
            while (true)
            {
                co_await winrt::resume_after(std::chrono::milliseconds(100));
                UpdateTimeDisplay();
            }
        }();
    }

    void MainWindow::StopTimer()
    {
        if (m_timer)
        {
            m_timer.Cancel();
            m_timer = nullptr;
        }
    }
}