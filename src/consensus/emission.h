// Copyright (c) 2024 The Pussycoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef PUSSYCOIN_CONSENSUS_EMISSION_H
#define PUSSYCOIN_CONSENSUS_EMISSION_H

#include <amount.h>
#include <stdint.h>

/** Get the threshold where tail emission begins */
uint64_t TailEmissionThreshold();

/** Calculate block reward using smooth emission schedule */
CAmount GetSmoothEmissionReward(uint64_t already_generated);

#endif // PUSSYCOIN_CONSENSUS_EMISSION_H 