/* Generated by re2c 3.0 on Sat May 13 10:39:57 2023 */

#ifndef RE2C_GENERATED_FUNCTIONS_H
#define RE2C_GENERATED_FUNCTIONS_H

#include <QStringView>
#include <QUuid>

namespace re2c {
    template < typename T >
    T strToUHex(QStringView v, bool &ok) {
        static_assert(std::is_unsigned < T > ::value,
                      "T must be an unsigned type");

        T value = 0;

        ok = false;
        for (int i = 0; i < v.length(); ++i) {
            const char ch = v[i].toLatin1();
            if (value > std::numeric_limits < T > ::max() / 16) {
                ok = false;
                return 0;
            }
            value *= 16;
            uchar digit = 0;
            switch (ch) {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9': {
                    digit = ch - '0';
                    ok    = true;
                    break;
                }
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f': {
                    digit = 10 + (ch - 'a');
                    ok    = true;
                    break;
                }
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F': {
                    digit = 10 + (ch - 'A');
                    ok    = true;
                    break;
                }
                default: {
                    ok = false;
                    return 0;
                }
            }

            if (value > (std::numeric_limits < T > ::max() - digit)) {
                ok = false;
                return 0;
            } else {
                value += digit;
            }
        }
        return value;
    }

    QStringView decimal(QStringView input);
    QStringView snbtNumber(QStringView input);
    QStringView itemSlot(QStringView input);
    QStringView nbtPathKey(QStringView input);
    QStringView resLocPart(QStringView input);
    QStringView objective(QStringView input);
    QStringView objectiveCriteria(QStringView input);
    QStringView realPlayerName(QStringView input);
    QStringView uuid(QStringView input, QUuid &result);
}

#endif // RE2C_GENERATED_FUNCTIONS_H
