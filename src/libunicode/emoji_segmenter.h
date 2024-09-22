/**
 * This file is part of the "libunicode" project
 *   Copyright (c) 2020 Christian Parpart <christian@parpart.family>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <libunicode/support.h>

#include <format>
#include <ostream>

namespace unicode
{

/// Used to distinguish between standard text and emoji text.
enum class PresentationStyle
{
    Text,
    Emoji
};

enum class EmojiSegmentationCategory : int8_t
{
    Invalid = -1,
    Emoji = 0,
    EmojiTextPresentation = 1,
    EmojiEmojiPresentation = 2,
    EmojiModifierBase = 3,
    EmojiModifier = 4,
    EmojiVSBase = 5,
    RegionalIndicator = 6,
    KeyCapBase = 7,
    CombiningEnclosingKeyCap = 8,
    CombiningEnclosingCircleBackslash = 9,
    ZWJ = 10,
    VS15 = 11,
    VS16 = 12,
    TagBase = 13,
    TagSequence = 14,
    TagTerm = 15,
};

/**
 * emoji_segmenter API for segmenting emojis into text-emoji and emoji-emoji presentations.
 *
 * This segmenter is segmenting emojis by their presentation property (text or emoji), that is,
 * whether an emoji is to be rendered in text mode or in emoji (colored) mode.
 *
 * It must be segmenting only emojis and not any other codepoints.
 */
class emoji_segmenter
{
  private:
    char32_t const* buffer_ = U"";
    size_t size_ = 0;

    size_t currentCursorBegin_ = 0;
    size_t currentCursorEnd_ = 0;
    size_t nextCursorBegin_ = 0;

    bool isEmoji_ = false;
    bool isNextEmoji_ = false;

  public:
    using property_type = PresentationStyle;

    constexpr emoji_segmenter() noexcept = default;
    constexpr emoji_segmenter& operator=(emoji_segmenter const&) noexcept = default;
    constexpr emoji_segmenter& operator=(emoji_segmenter&&) noexcept = default;
    constexpr emoji_segmenter(emoji_segmenter const&) noexcept = default;
    constexpr emoji_segmenter(emoji_segmenter&&) noexcept = default;

    emoji_segmenter(char32_t const* buffer, size_t size) noexcept;

    emoji_segmenter(std::u32string_view const& sv) noexcept: emoji_segmenter(sv.data(), sv.size()) {}

    constexpr char32_t const* buffer() const noexcept { return buffer_; }
    constexpr size_t size() const noexcept { return size_; }
    constexpr size_t currentCursorBegin() const noexcept { return currentCursorBegin_; }
    constexpr size_t currentCursorEnd() const noexcept { return currentCursorEnd_; }

    bool consume(out<size_t> size, out<PresentationStyle> emoji) noexcept;

    /// @returns whether or not the currently segmented emoji is to be rendered in text-presentation or not.
    constexpr bool isText() const noexcept { return !isEmoji_; }

    /// @returns whether or not the currently segmented emoji is to be rendered in emoji-presentation or not.
    constexpr bool isEmoji() const noexcept { return isEmoji_; }

    /// @returns the underlying current segment that has been processed the last.
    constexpr std::u32string_view substr() const noexcept
    {
        // TODO: provide such an accessor in text_run_segmenter
        if (currentCursorEnd_ > 0)
            return std::u32string_view(buffer_ + currentCursorBegin_, currentCursorEnd_ - currentCursorBegin_);
        else
            return std::u32string_view {};
    }

    /// @returns the underlying current segment that has been processed the last.
    constexpr std::u32string_view operator*() const noexcept { return substr(); }

  private:
    size_t consume_once();
};

inline std::ostream& operator<<(std::ostream& os, PresentationStyle ps)
{
    switch (ps)
    {
        case PresentationStyle::Text: return os << "Text";
        case PresentationStyle::Emoji: return os << "Emoji";
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, EmojiSegmentationCategory value)
{
    switch (value)
    {
        case unicode::EmojiSegmentationCategory::Invalid: return os << "Invalid";
        case unicode::EmojiSegmentationCategory::Emoji: return os << "Emoji";
        case unicode::EmojiSegmentationCategory::EmojiTextPresentation: return os << "EmojiTextPresentation";
        case unicode::EmojiSegmentationCategory::EmojiEmojiPresentation: return os << "EmojiEmojiPresentation";
        case unicode::EmojiSegmentationCategory::EmojiModifierBase: return os << "EmojiModifierBase";
        case unicode::EmojiSegmentationCategory::EmojiModifier: return os << "EmojiModifier";
        case unicode::EmojiSegmentationCategory::EmojiVSBase: return os << "EmojiVSBase";
        case unicode::EmojiSegmentationCategory::RegionalIndicator: return os << "RegionalIndicator";
        case unicode::EmojiSegmentationCategory::KeyCapBase: return os << "KeyCapBase";
        case unicode::EmojiSegmentationCategory::CombiningEnclosingKeyCap: return os << "CombiningEnclosingKeyCap";
        case unicode::EmojiSegmentationCategory::CombiningEnclosingCircleBackslash:
            return os << "CombiningEnclosingCircleBackslash";
        case unicode::EmojiSegmentationCategory::ZWJ: return os << "ZWJ";
        case unicode::EmojiSegmentationCategory::VS15: return os << "VS15";
        case unicode::EmojiSegmentationCategory::VS16: return os << "VS16";
        case unicode::EmojiSegmentationCategory::TagBase: return os << "TagBase";
        case unicode::EmojiSegmentationCategory::TagSequence: return os << "TagSequence";
        case unicode::EmojiSegmentationCategory::TagTerm: return os << "TagTerm";
    }
    return os;
}

} // namespace unicode

template <>
struct std::formatter<unicode::PresentationStyle>: std::formatter<std::string_view>
{
    template <typename FormatContext>
    auto format(unicode::PresentationStyle value, FormatContext& ctx) const -> FormatContext::iterator
    {
        string_view name;
        switch (value)
        {
            case unicode::PresentationStyle::Text: name = "Text"; break;
            case unicode::PresentationStyle::Emoji: name = "Emoji"; break;
        }
        return formatter<string_view>::format(name, ctx);
    }
};

template <>
struct std::formatter<unicode::EmojiSegmentationCategory>: std::formatter<std::string_view>
{
    template <typename FormatContext>
    auto format(unicode::EmojiSegmentationCategory value, FormatContext& ctx) const -> FormatContext::iterator
    {
        using unicode::EmojiSegmentationCategory;
        string_view name;
        switch (value)
        {
            case EmojiSegmentationCategory::Invalid: name = "Invalid"; break;
            case EmojiSegmentationCategory::Emoji: name = "Emoji"; break;
            case EmojiSegmentationCategory::EmojiTextPresentation: name = "EmojiTextPresentation"; break;
            case EmojiSegmentationCategory::EmojiEmojiPresentation: name = "EmojiEmojiPresentation"; break;
            case EmojiSegmentationCategory::EmojiModifierBase: name = "EmojiModifierBase"; break;
            case EmojiSegmentationCategory::EmojiModifier: name = "EmojiModifier"; break;
            case EmojiSegmentationCategory::EmojiVSBase: name = "EmojiVSBase"; break;
            case EmojiSegmentationCategory::RegionalIndicator: name = "RegionalIndicator"; break;
            case EmojiSegmentationCategory::KeyCapBase: name = "KeyCapBase"; break;
            case EmojiSegmentationCategory::CombiningEnclosingKeyCap: name = "CombiningEnclosingKeyCap"; break;
            case EmojiSegmentationCategory::CombiningEnclosingCircleBackslash: name = "CombiningEnclosingCircleBackslash"; break;
            case EmojiSegmentationCategory::ZWJ: name = "ZWJ"; break;
            case EmojiSegmentationCategory::VS15: name = "VS15"; break;
            case EmojiSegmentationCategory::VS16: name = "VS16"; break;
            case EmojiSegmentationCategory::TagBase: name = "TagBase"; break;
            case EmojiSegmentationCategory::TagSequence: name = "TagSequence"; break;
            case EmojiSegmentationCategory::TagTerm: name = "TagTerm"; break;
        }
        return formatter<string_view>::format(name, ctx);
    }
};
