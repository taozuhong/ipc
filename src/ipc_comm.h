#ifndef IPC_COMM_H
#define IPC_COMM_H

#include <sys/types.h>
#include <cstdint>
#include <iostream>


const char     App_Usage[] =
" {OPTIONS} \n\
\n\
utils to demo IPC methods\n\
\n\
OPTIONS: \n\
-h               Show help \n\
-shm             Enter share memory mode\n\
-pipe            Enter pipe mode(anony pipe)\n\
-fifo            Enter fifo mode(named pipe)\n\
-signal          Enter signal mode(1-none;2-args;3-customize)\n\
-msg             Enter message mode\n\
\n\
Welcome to contribute and share with your friends.";


const char      SHM_SVR_TEXT[] = "SERVER: 在这之前，我们可以从中兴通讯在2017年3月7日的公告及美国司法部公布的判决书中看到，中兴通讯因在2010年1月至2016年3月期间，共向朝鲜和伊朗出口400余件原产美国的通讯装备，这违反了《国际紧急经济权力法》（International Emergency Economic Powers Act，IEEPA）。中兴通讯被指控罪名还包括违反了《美国出口管理条例》（Export Administration Act ，EAA）和《伊朗交易与制裁条例》（ Iranian Transaction and Sanctions Regulations，ITSR）。2017年3月，中兴通讯与美国政府就出口管制调查案件达成和解。从和解协议来看，中兴受到指控主要包括三个方面，一是中兴直接或间接将美国商品出口销售给美国制裁的国家——伊朗；二是通过第三方国家将《美国出口管理条例》项下受管制的美国原产商品进行再出口至伊朗；三是中兴知晓违反美国出口管制的后果，试图规避或者逃避，密谋违反、并促使违反伊朗制裁措施的活动。为达成和解，中兴通讯将支付约8.9亿美元的刑事和民事罚金（其中，向美国商务部工业与安全局缴纳的行政罚款3.61亿元；向美国司法部缴纳的刑事罚款和没收款项为4.3亿多美元；向美国财政部海外资产管理办公室缴纳的行政罚款1亿多美元）。此外，美国商务部工业与安全局对中兴通讯的3亿美元罚金被暂缓，是否支付将依据未来七年中兴公司对协议的遵守，并继续接受独立的合规监管和审计的结果而确定。这次事件让中兴通讯成为违反美国出口管制而被罚款金额最大的中国公司，同时也是中国企业收到的来自美国政府处罚金额最高的一张罚单。";
const char      SHM_CLT_TEXT[] = "CLIENT: 通用电气公司（GE）可以存续百年，不是仅仅因为公司拥有先进的技术或管理，更是因为公司优秀的企业文化，其中最重要的是以诚信与合规作为公司的文化，GE将合规作为企业发展的基石。首先，GE将诚信合规定义成为企业的文化，该公司的核心价值观就是诚信。GE要求公司的领导层不仅要为他们的行为负责，还要为企业培育合规诚信的文化负责。其次，GE将诚信(integrity)作为合规的主要含义。GE不仅仅满足于遵守法律法规，而是要以诚信为原则开展业务。除了遵守法律法规外，GE还要求员工不能欺骗商业伙伴或违反商业道德，即使这些商业道德并不属于法律管辖的范围。第三，GE永远要走在法律法规管制的前面。GE不是简单地应对法律法规，不是被动地守法，而是更加主动地超越法律的要求来要求自己，而不是触犯了法律被惩罚以后才去做合规。另外，作为一家在全球开展业务的公司，GE面临着来自全球不同国家和地区的法律法规的监管，必须以最高的标准来要求自己，不能因为某个国家或者某个地区的标准低就降低公司在该国家或者地区的合规标准。第四，GE把主动合规作为合规工作的主要目标。GE强调合规经营，不仅仅是为了避免相应的处罚措施，更是要在全球竞争下打造企业的核心竞争力，不要做只会赚钱的公司。GE把主动的守法当作合规管理的主要目标，因此，对合规经营认识就比一般的公司更为深刻。";


const int64_t   SHM_SIZE = 1024 * 1024 * 16;
const char      IPC_SHM_NAME[] = "/ipc_test_first";
const char      IPC_PIPE_SVR_READ[] = "/tmp/ipc_pipe_read";
const char      IPC_PIPE_SVR_WRITE[] = "/tmp/ipc_pipe_write";
const uint32_t  IPC_PIPE_PERMISSION = 0666;         // all: read + write

const char      SHM_HEAD_SIZE = 8;
const char      SHM_DATA_SIZE = 8;
const uint64_t  SHM_FLAG_LOCK = 0xFFFFFFFFFFFFFFFF;
const uint64_t  SHM_FLAG_UNLOCK = 0xEEEEEEEEEEEEEEEE;
const uint64_t  SHM_FLAG_RELEASE = 0xAAAAAAAAAAAAAAAA;
const uint8_t   PIPE_STDIN = 0;
const uint8_t   PIPE_STDOUT = 1;

template <typename _ITp>
_ITp atomic_read(void * ptr) {
    (void)(ptr);
    _ITp temp_value = 0;
    __atomic_load((_ITp *)ptr, &temp_value, __ATOMIC_SEQ_CST);
    return  temp_value;
}

template <typename _ITp>
void atomic_write(void * ptr, _ITp value) {
    (void)(ptr);
    __atomic_store((_ITp *)ptr, &value, __ATOMIC_SEQ_CST);
}


#endif
