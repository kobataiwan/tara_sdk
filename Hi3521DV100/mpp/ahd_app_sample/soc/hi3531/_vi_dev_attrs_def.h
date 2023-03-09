#include <nc_hi_vivo.h>

static VI_DEV_ATTR_S DEV_ATTR_BT656D1_4MUX =
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_4Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};

static VI_DEV_ATTR_S DEV_ATTR_BT656D1_2MUX =
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_2Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};

static VI_DEV_ATTR_S DEV_ATTR_BT656D1_1MUX =
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};

static VI_DEV_ATTR_S DEV_ATTR_BT656_P_1MUX =
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};

static VI_DEV_ATTR_S DEV_ATTR_OV2715_BT601_1080P =
/*����ʱ��1:OV2715 BT601 1080p@30fps����ʱ��*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_4Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};

static VI_DEV_ATTR_S DEV_ATTR_7441_BT1120_1080P =
/* ����ʱ��3:7441 BT1120 1080P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_4Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};

//Hwarang
static VI_DEV_ATTR_S DEV_ATTR_6114_BT656_1MUX_720P =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};
static VI_DEV_ATTR_S DEV_ATTR_6114_BT656_1MUX_720P_CIF =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};
static VI_DEV_ATTR_S DEV_ATTR_6114_BT656_1MUX_960P_NT =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};
static VI_DEV_ATTR_S DEV_ATTR_6114_BT656_1MUX_960P_PAL =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};
static VI_DEV_ATTR_S DEV_ATTR_6114_BT656_4MUX_960P_NT =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_4Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};
static VI_DEV_ATTR_S DEV_ATTR_6114_BT656_4MUX_960P_PAL =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_4Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};
static VI_DEV_ATTR_S DEV_ATTR_6114_BT656_1MUX_1280P_NT =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};
static VI_DEV_ATTR_S DEV_ATTR_6114_BT656_1MUX_1280P_PAL =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};
static VI_DEV_ATTR_S DEV_ATTR_6114_BT656_1MUX_1440P_NT =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};
static VI_DEV_ATTR_S DEV_ATTR_6114_BT656_1MUX_1440P_PAL =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};
static VI_DEV_ATTR_S DEV_ATTR_6114_BT656_1MUX_1920P_NT =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};
static VI_DEV_ATTR_S DEV_ATTR_6114_BT656_1MUX_1920P_PAL =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};
static VI_DEV_ATTR_S DEV_ATTR_6114_BT656_2MUX_960H_CIF =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_2Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};
static VI_DEV_ATTR_S DEV_ATTR_6114_BT656_2MUX_720P_CIF =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_2Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};
//tony test 05.20
static VI_DEV_ATTR_S DEV_ATTR_6114_BT656_1MUX_1080P =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};
static VI_DEV_ATTR_S DEV_ATTR_6114_BT656_1MUX_1080P_CIF =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};
static VI_DEV_ATTR_S DEV_ATTR_6114_BT656_2MUX_1080P_CIF =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_2Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};

static VI_DEV_ATTR_S DEV_ATTR_6114_BT656_2MUX_720P =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_2Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};

static VI_DEV_ATTR_S DEV_ATTR_6114_BT656_4MUX_720P =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_4Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_DOUBLE,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};

static VI_DEV_ATTR_S DEV_ATTR_BT1120_SINGLEUP=
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT1120_STANDARD,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,
};

static VI_DEV_ATTR_S DEV_ATTR_BT656_1MUX_QHD =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_DOUBLE,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    //VI_INPUT_DATA_YVYU,
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};

static VI_DEV_ATTR_S DEV_ATTR_BT1120_1MUX_QHD =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
	/*�ӿ�ģʽ*/
	VI_MODE_BT1120_STANDARD,
	/*1��2��4·����ģʽ*/
	VI_WORK_MODE_1Multiplex,
	/* r_mask    g_mask    b_mask*/
	{0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_DOUBLE,
};

static VI_DEV_ATTR_S DEV_ATTR_BT656_1MUX_QHDX =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    //VI_INPUT_DATA_YVYU,
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};

static VI_DEV_ATTR_S DEV_ATTR_BT656_1MUX_UHDX =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_DOUBLE,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};

static VI_DEV_ATTR_S DEV_ATTR_BT656_1MUX_SINGLE=
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};

static VI_DEV_ATTR_S DEV_ATTR_BT656_SINGLEUP=
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};

static VI_DEV_ATTR_S DEV_ATTR_7441_INTERLEAVED_720P =
/* ����ʱ��3:7441 BT1120 720P@60fps����ʱ�� (�Խ�ʱ��: ʱ��)*/
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};

static VI_DEV_ATTR_S DEV_ATTR_BT656_1080P_2MUX_DUALEDGE =
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_2Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_DOUBLE,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};

static VI_DEV_ATTR_S DEV_ATTR_BT656_1080P_2MUX_SINGLE =
{
    /*�ӿ�ģʽ*/
    VI_MODE_BT656,
    /*1��2��4·����ģʽ*/
    VI_WORK_MODE_2Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* ˫������ʱ�������� */
	VI_CLK_EDGE_SINGLE_UP,

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, ��֧��YUV��ʽ*/
    VI_INPUT_DATA_YVYU,
    /*ͬ����Ϣ����Ӧreg�ֲ����������, --bt1120ʱ����Ч*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing��Ϣ����Ӧreg�ֲ����������*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*ʹ���ڲ�ISP*/
    VI_PATH_BYPASS,
    /*������������*/
    VI_DATA_TYPE_YUV
};

