package com.google.android.exoplayer2.upstream;

/**
 * Created by roy on 23-Feb-16.
 */
public class ZixiStatistics {
    public long    packets;
    public long    bytes;
    public long    outOfOrder;
    public long    dropped;
    public long    duplicates;
    public long    overflow;
    public int     bitRate;
    public int     packetRate;
    public int     jitter;
    public int     rtt;
    public int     latency;
    public int     availableBitrate;
    public boolean congested;

    public long    arqPackets;
    public long    fecPackets;
    public long    arqRecovered;
    public long    fecRecovered;
    public long    notRecovered;
    public long    ecDuplicates;
    public long    ecRequests;
    public long    ecOverflow;
    public int     fecBitrate;
    public int     fecPacketRate;
    public long    nullsStuffed;
    public void fromLongArray(long[] statisticsBuffer) {
        packets = statisticsBuffer[0];
        bytes = statisticsBuffer[1];
        outOfOrder = statisticsBuffer[2];
        dropped = statisticsBuffer[3];
        duplicates = statisticsBuffer[4];
        overflow = statisticsBuffer[5];
        bitRate = (int)statisticsBuffer[6];
        packetRate = (int)statisticsBuffer[7];
        jitter = (int)statisticsBuffer[8];
        rtt = (int)statisticsBuffer[9];
        latency = (int)statisticsBuffer[10];
        availableBitrate = (int)statisticsBuffer[11];
        congested = statisticsBuffer[12] != 0;

        arqPackets = statisticsBuffer[13];
        fecPackets = statisticsBuffer[14];
        arqRecovered = statisticsBuffer[15];
        fecRecovered = statisticsBuffer[16];
        notRecovered = statisticsBuffer[17];
        ecDuplicates = statisticsBuffer[18];
        ecRequests = statisticsBuffer[19];
        ecOverflow = statisticsBuffer[20];
        fecBitrate = (int)statisticsBuffer[21];
        fecPacketRate = (int)statisticsBuffer[22];
        nullsStuffed = statisticsBuffer[23];
    }
}
