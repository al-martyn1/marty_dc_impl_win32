// Inline methods. Not for include itself

//----------------------------------------------------------------------------
virtual Endianness getHostEndianness() const override
{
#if defined(MARTY_IDC_ARCH_LITTLE_ENDIAN)

    return Endianness::littleEndian;

#else

    return Endianness::bigEndian;

#endif
}

//----------------------------------------------------------------------------
virtual std::uint8_t* swapByteOrder(std::uint8_t *pData, std::size_t dataSize) const override
{
    if (dataSize<2)
    {
        return pData; // nothing to swap
    }

    std::uint8_t *pRes = pData;

    std::uint8_t *pLastData = pData + (std::intptr_t)(dataSize - 1);

    const std::size_t iMax = dataSize/2;
    for(std::size_t i=0u; i!=iMax /* pData<pLastData */ ; ++pData, --pLastData)
    {
        std::swap(*pData, *pLastData);
    }

    return pRes;
}

//----------------------------------------------------------------------------
virtual std::uint8_t* convertEndiannessToHost  (std::uint8_t *pData, std::size_t dataSize, Endianness srcEndianness) const override
{
    if (getHostEndianness()!=srcEndianness)
    {
        swapByteOrder(pData, dataSize);
    }

    return pData;
}

//----------------------------------------------------------------------------
virtual std::uint8_t* convertEndiannessFromHost(std::uint8_t *pData, std::size_t dataSize, Endianness dstEndianness) const override
{
    if (getHostEndianness()!=dstEndianness)
    {
        swapByteOrder(pData, dataSize);
    }

    return pData;
}


