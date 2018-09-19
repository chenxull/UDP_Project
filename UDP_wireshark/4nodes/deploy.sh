#!/bin/bash

scp OneSliceAcqDecode.sh uctworker@192.168.12.171:~/Program/Capture/OneSliceAcqDecode.sh
scp emptyPatientArchive.sh uctworker@192.168.12.171:~/Program/Capture/emptyPatientArchive.sh
scp UDPCatch1.sh uctworker@192.168.12.171:~/Program/Capture/UDPCatch.sh
scp UDPCatch2.sh uctworker@192.168.12.172:~/Program/Capture/UDPCatch.sh
scp UDPCatch3.sh uctworker@192.168.12.173:~/Program/Capture/UDPCatch.sh
scp UDPCatch4.sh uctworker@192.168.12.174:~/Program/Capture/UDPCatch.sh
echo "scp done."
ssh uctworker@192.168.12.171 "chmod +x ~/Program/Capture/OneSliceAcqDecode.sh"
ssh uctworker@192.168.12.171 "chmod +x ~/Program/Capture/emptyPatientArchive.sh"
ssh uctworker@192.168.12.171 "chmod +x ~/Program/Capture/UDPCatch.sh"
ssh uctworker@192.168.12.172 "chmod +x ~/Program/Capture/UDPCatch.sh"
ssh uctworker@192.168.12.173 "chmod +x ~/Program/Capture/UDPCatch.sh"
ssh uctworker@192.168.12.174 "chmod +x ~/Program/Capture/UDPCatch.sh"
