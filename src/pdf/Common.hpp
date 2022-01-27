//
// Created by alvin on 27.01.2022.
//

#ifndef PDF_SANITIZER_COMMON_HPP
#define PDF_SANITIZER_COMMON_HPP

#ifdef NODISCARD
#undef NODISCARD
#endif
#define NODISCARD [[nodiscard]]

#ifdef MAYBE_UNUSED
#undef MAYBE_UNUSED
#endif
#define MAYBE_UNUSED [[maybe_unused]]

#endif
