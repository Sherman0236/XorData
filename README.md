# XorData
 
This project is an open-source framework to enable obfuscation of constants, variables, and strings.

Example output of obfuscated data can be found [here](https://github.com/Sherman0236/XorData/blob/master/assets/assets.md).
My blog post describing project the project can be found [here](https://sherman0236.github.io/posts/data-obf/).

## Features
* Compile time constant obfuscation 
* Compile time string obfuscation
* Run time variable obfuscation
* Unique encryption keys per compilation and usage 
* 64-bit FNV hashing
* Utilizes SIMD (SSE)

## Disclaimer

The obfuscation techniques implemented in this project are not intended to be foolproof. They serve as a foundation that should be enhanced further. The goal of this project is not to provide absolute protection but to obfuscate data effectively. Please note the provided code is designed for MSVC and requires C++17 or newer.

## License

This project is licensed under the terms of the Mozilla Public License 2.0.

The Mozilla Public License 2.0 is a permissive open-source software license that allows you to use, modify, and distribute the software, as long as you include the original license and copyright notice in any copies or derivative works. The license also includes some additional terms and conditions, such as the requirement to disclose source code changes, and the prohibition of using the software for certain types of purposes, such as creating nuclear weapons.

For more information about the Mozilla Public License 2.0, please see the full license text [here](https://www.mozilla.org/en-US/MPL/2.0/).