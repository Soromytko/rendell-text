#pragma once

#define RENDELL_TEXT_DECLARE_SHARED_PTR(ClassName)                                                 \
    typedef std::shared_ptr<ClassName> ClassName##SharedPtr;

#define RENDELL_TEXT_DECLARE_SHARED_PTR_FACTORY(ClassName)                                         \
    RENDELL_TEXT_DECLARE_SHARED_PTR(ClassName)                                                     \
    template <typename... Args> ClassName##SharedPtr make##ClassName(Args &&...args) {             \
        return std::make_shared<ClassName>(std::forward<Args>(args)...);                           \
    }
