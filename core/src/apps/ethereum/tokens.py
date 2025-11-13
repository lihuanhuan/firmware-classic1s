# generated from tokens.py.mako
# (by running `make templates` in `core`)
# do not edit manually!
# fmt: off

# NOTE: returning a tuple instead of `TokenInfo` from the "data" function
# saves 5600 bytes of flash size. Implementing the `_token_iterator`
# instead of if-tree approach saves another 5600 bytes.

# NOTE: interestingly, it did not save much flash size to use smaller
# parts of the address, for example address length of 10 bytes saves
# 1 byte per entry, so 1887 bytes overall (and further decrease does not help).
# (The idea was not having to store the whole address, even a smaller part
# of it has enough collision-resistance.)
# (In the if-tree approach the address length did not have any effect whatsoever.)

from typing import Iterator


class TokenInfo:
    def __init__(self, symbol: str, decimals: int) -> None:
        self.symbol = symbol
        self.decimals = decimals


UNKNOWN_TOKEN = TokenInfo("Wei UNKN", 0)


def token_by_chain_address(chain_id: int, address: bytes) -> TokenInfo:
    for addr, symbol, decimal in _token_iterator(chain_id):
        if address == addr:
            return TokenInfo(symbol, decimal)
    return UNKNOWN_TOKEN


def _token_iterator(chain_id: int) -> Iterator[tuple[bytes, str, int]]:
    if chain_id == 1:
        yield (  # address, symbol, decimals
            b"\x7f\xc6\x65\x00\xc8\x4a\x76\xad\x7e\x9c\x93\x43\x7b\xfc\x5a\xc3\x3e\x2d\xda\xe9",
            "AAVE",
            18,
        )
        yield (  # address, symbol, decimals
            b"\xbb\x0e\x17\xef\x65\xf8\x2a\xb0\x18\xd8\xed\xd7\x76\xe8\xdd\x94\x03\x27\xb2\x8b",
            "AXS",
            18,
        )
        yield (  # address, symbol, decimals
            b"\xba\x10\x00\x00\x62\x5a\x37\x54\x42\x39\x78\xa6\x0c\x93\x17\xc5\x8a\x42\x4e\x3d",
            "BAL",
            18,
        )
        yield (  # address, symbol, decimals
            b"\x0d\x87\x75\xf6\x48\x43\x06\x79\xa7\x09\xe9\x8d\x2b\x0c\xb6\x25\x0d\x28\x87\xef",
            "BAT",
            18,
        )
        yield (  # address, symbol, decimals
            b"\x1a\x4b\x46\x69\x6b\x2b\xb4\x79\x4e\xb3\xd4\xc2\x6f\x1c\x55\xf9\x17\x0f\xa4\xc5",
            "BIT",
            18,
        )
        yield (  # address, symbol, decimals
            b"\x4f\xab\xb1\x45\xd6\x46\x52\xa9\x48\xd7\x25\x33\x02\x3f\x6e\x7a\x62\x3c\x7c\x53",
            "BUSD",
            18,
        )
        yield (  # address, symbol, decimals
            b"\xc0\x0e\x94\xcb\x66\x2c\x35\x20\x28\x2e\x6f\x57\x17\x21\x40\x04\xa7\xf2\x68\x88",
            "COMP",
            18,
        )
        yield (  # address, symbol, decimals
            b"\x6b\x17\x54\x74\xe8\x90\x94\xc4\x4d\xa9\x8b\x95\x4e\xed\xea\xc4\x95\x27\x1d\x0f",
            "DAI",
            18,
        )
        yield (  # address, symbol, decimals
            b"\xf6\x29\xcb\xd9\x4d\x37\x91\xc9\x25\x01\x52\xbd\x8d\xfb\xdf\x38\x0e\x2a\x3b\x9c",
            "ENJ",
            18,
        )
        yield (  # address, symbol, decimals
            b"\x4e\x15\x36\x1f\xd6\xb4\xbb\x60\x9f\xa6\x3c\x81\xa2\xbe\x19\xd8\x73\x71\x78\x70",
            "FTM",
            18,
        )
        yield (  # address, symbol, decimals
            b"\x68\x10\xe7\x76\x88\x0c\x02\x93\x3d\x47\xdb\x1b\x9f\xc0\x59\x08\xe5\x38\x6b\x96",
            "GNO",
            18,
        )
        yield (  # address, symbol, decimals
            b"\xc9\x44\xe9\x0c\x64\xb2\xc0\x76\x62\xa2\x92\xbe\x62\x44\xbd\xf0\x5c\xda\x44\xa7",
            "GRT",
            18,
        )
        yield (  # address, symbol, decimals
            b"\x05\x6f\xd4\x09\xe1\xd7\xa1\x24\xbd\x70\x17\x45\x9d\xfe\xa2\xf3\x87\xb6\xd5\xcd",
            "GUSD",
            2,
        )
        yield (  # address, symbol, decimals
            b"\x6f\x25\x96\x37\xdc\xd7\x4c\x76\x77\x81\xe3\x7b\xc6\x13\x3c\xd6\xa6\x8a\xa1\x61",
            "HT",
            18,
        )
        yield (  # address, symbol, decimals
            b"\x6f\xb3\xe0\xa2\x17\x40\x7e\xff\xf7\xca\x06\x2d\x46\xc2\x6e\x5d\x60\xa1\x4d\x69",
            "IOTX",
            18,
        )
        yield (  # address, symbol, decimals
            b"\xbb\xbb\xca\x6a\x90\x1c\x92\x6f\x24\x0b\x89\xea\xcb\x64\x1d\x8a\xec\x7a\xea\xfd",
            "LRC",
            18,
        )
        yield (  # address, symbol, decimals
            b"\x7d\x1a\xfa\x7b\x71\x8f\xb8\x93\xdb\x30\xa3\xab\xc0\xcf\xc6\x08\xaa\xcf\xeb\xb0",
            "MATIC",
            18,
        )
        yield (  # address, symbol, decimals
            b"\x9f\x8f\x72\xaa\x93\x04\xc8\xb5\x93\xd5\x55\xf1\x2e\xf6\x58\x9c\xc3\xa5\x79\xa2",
            "MKR",
            18,
        )
        yield (  # address, symbol, decimals
            b"\xb6\x21\x32\xe3\x5a\x6c\x13\xee\x1e\xe0\xf8\x4d\xc5\xd4\x0b\xad\x8d\x81\x52\x06",
            "NEXO",
            18,
        )
        yield (  # address, symbol, decimals
            b"\x4f\xe8\x32\x13\xd5\x63\x08\x33\x0e\xc3\x02\xa8\xbd\x64\x1f\x1d\x01\x13\xa4\xcc",
            "NU",
            18,
        )
        yield (  # address, symbol, decimals
            b"\x75\x23\x1f\x58\xb4\x32\x40\xc9\x71\x8d\xd5\x8b\x49\x67\xc5\x11\x43\x42\xa8\x6c",
            "OKB",
            18,
        )
        yield (  # address, symbol, decimals
            b"\xd2\x61\x14\xcd\x6e\xe2\x89\xac\xcf\x82\x35\x0c\x8d\x84\x87\xfe\xdb\x8a\x0c\x07",
            "OMG",
            18,
        )
        yield (  # address, symbol, decimals
            b"\x95\xad\x61\xb0\xa1\x50\xd7\x92\x19\xdc\xf6\x4e\x1e\x6c\xc0\x1f\x0b\x64\xc4\xce",
            "SHIB",
            18,
        )
        yield (  # address, symbol, decimals
            b"\xc0\x11\xa7\x3e\xe8\x57\x6f\xb4\x6f\x5e\x1c\x57\x51\xca\x3b\x9f\xe0\xaf\x2a\x6f",
            "SNX",
            18,
        )
        yield (  # address, symbol, decimals
            b"\x6b\x35\x95\x06\x87\x78\xdd\x59\x2e\x39\xa1\x22\xf4\xf5\xa5\xcf\x09\xc9\x0f\xe2",
            "SUSHI",
            18,
        )
        yield (  # address, symbol, decimals
            b"\x00\x00\x00\x00\x00\x08\x5d\x47\x80\xb7\x31\x19\xb6\x44\xae\x5e\xcd\x22\xb3\x76",
            "TUSD",
            18,
        )
        yield (  # address, symbol, decimals
            b"\x1f\x98\x40\xa8\x5d\x5a\xf5\xbf\x1d\x17\x62\xf9\x25\xbd\xad\xdc\x42\x01\xf9\x84",
            "UNI",
            18,
        )
        yield (  # address, symbol, decimals
            b"\xa0\xb8\x69\x91\xc6\x21\x8b\x36\xc1\xd1\x9d\x4a\x2e\x9e\xb0\xce\x36\x06\xeb\x48",
            "USDC",
            6,
        )
        yield (  # address, symbol, decimals
            b"\x8e\x87\x0d\x67\xf6\x60\xd9\x5d\x5b\xe5\x30\x38\x0d\x0e\xc0\xbd\x38\x82\x89\xe1",
            "USDP",
            18,
        )
        yield (  # address, symbol, decimals
            b"\xda\xc1\x7f\x95\x8d\x2e\xe5\x23\xa2\x20\x62\x06\x99\x45\x97\xc1\x3d\x83\x1e\xc7",
            "USDT",
            6,
        )
        yield (  # address, symbol, decimals
            b"\x22\x60\xfa\xc5\xe5\x54\x2a\x77\x3a\xa4\x4f\xbc\xfe\xdf\x7c\x19\x3b\xc2\xc5\x99",
            "WBTC",
            8,
        )
        yield (  # address, symbol, decimals
            b"\x0b\xc5\x29\xc0\x0c\x64\x01\xae\xf6\xd2\x20\xbe\x8c\x6e\xa1\x66\x7f\x6a\xd9\x3e",
            "YFI",
            18,
        )
        yield (  # address, symbol, decimals
            b"\xe4\x1d\x24\x89\x57\x1d\x32\x21\x89\x24\x6d\xaf\xa5\xeb\xde\x1f\x46\x99\xf4\x98",
            "ZRX",
            18,
        )
    if chain_id == 43114:
        yield (  # address, symbol, decimals
            b"\x97\x02\x23\x0a\x8e\xa5\x36\x01\xf5\xcd\x2d\xc0\x0f\xdb\xc1\x3d\x4d\xf4\xa8\xc7",
            "USDT",
            6,
        )
        yield (  # address, symbol, decimals
            b"\xb3\x1f\x66\xaa\x3c\x1e\x78\x53\x63\xf0\x87\x5a\x1b\x74\xe2\x7b\x85\xfd\x66\xc7",
            "WAVAX",
            18,
        )
