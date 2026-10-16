#pragma once

#include <array>

#include "Activation/CaacActivationCodec.h"
#include "monocypher-ed25519.h"

namespace CaacActivationTestSupport {
// Public RFC 8032 test seed; never used by the application or the issuer's real key.
inline QByteArray testSeed()
{
    return QByteArray::fromHex("9d61b19deffd5a60ba844af492ec2cc44449c5697b326919703bac031cae7f60");
}

inline QByteArray publicKey()
{
    return QByteArray::fromHex("d75a980182b10ab7d54bfed3c964073a0ee172f3daa62325af021a68f707511a");
}

inline QString activationCode(const QByteArray& machineIdentity)
{
    auto seed = testSeed();
    std::array<uint8_t, 64> secret{};
    std::array<uint8_t, 32> pub{};
    crypto_ed25519_key_pair(secret.data(), pub.data(), reinterpret_cast<uint8_t*>(seed.data()));
    const auto hash = CaacActivationCodec::machineHash(machineIdentity);
    const auto message = CaacActivationCodec::signedMessage(hash);
    std::array<uint8_t, 64> signature{};
    crypto_ed25519_sign(signature.data(), secret.data(), reinterpret_cast<const uint8_t*>(message.constData()),
                        message.size());
    crypto_wipe(secret.data(), secret.size());
    return CaacActivationCodec::encode(hash,
                                       QByteArray(reinterpret_cast<const char*>(signature.data()), signature.size()));
}
}  // namespace CaacActivationTestSupport
