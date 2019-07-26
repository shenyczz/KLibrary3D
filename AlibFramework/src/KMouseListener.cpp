#include "stdafx.h"
#include "KMouseListener.h"


KMouseListener::KMouseListener()
{

}

KMouseListener::KMouseListener(LPVOID pOwner)
{
	m_pOwner = pOwner;
}

KMouseListener::~KMouseListener()
{
}

/*

public abstract class MouseListenerBase : Target, IMouseListener
{
    protected MouseListenerBase()
    {
        m_pointSequence = new List<POINT>();   // ������(����ͼ��ʹ��)
        IsFinishedGeometry = true;              // �Ƿ����ͼ��(��ʼ״̬Ϊ���ͼ��)
    }

    protected static IVision s_target = null;
    protected List<POINT> m_pointSequence;

    #region Enevt

    //envet Primivite
    public event EventHandler PrimiviteComplated;
    protected void Raise_PrimiviteComplated(Object s, EventArgs e)
    {
        PrimiviteComplated?.Invoke(s, e);
    }

    #endregion

    #region Properties
    /// <summary>
    /// ��ǰ�Ӿ�
    /// </summary>
    public IVision CurrentVision { get; set; }

    /// <summary>
    /// ������λ�� LastMousePosition
    /// </summary>
    public POINT LastPoint { get; set; }

    public POINT RLastPoint { get; set; }

    /// <summary>
    /// ����������ʱλ��(�����϶�)
    /// </summary>
    public POINT LButtonPoint { get; set; }

    /// <summary>
    /// ����Ҽ�����ʱλ��
    /// </summary>
    public POINT RButtonPoint { get; set; }

    /// <summary>
    /// ����϶�
    /// </summary>
    public Boolean IsDragging { get; set; }

    /// <summary>
    /// ����
    /// </summary>
    public Boolean IsWheeling { get; set; }

    /// <summary>
    /// �Ƿ�˫��
    /// </summary>
    public Boolean IsDoubleClick { get; set; }

    /// <summary>
    /// �������
    /// </summary>
    public Boolean IsLeftButtonDown { get; set; }

    /// <summary>
    /// �Ҽ�����
    /// </summary>
    public Boolean IsRightButtonDown { get; set; }

    /// <summary>
    /// ���ͼ�Σ��������ߵȶ��ͼ��ʱ�õ���
    /// </summary>
    public Boolean IsFinishedGeometry { get; set; }

    #endregion


    #region Public Functions - Mouse Events handdler

    /// <summary>
    /// ��갴��
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    public abstract void MouseDown(Object sender, EventArgs e);

    /// <summary>
    /// ����ͷ�
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    public abstract void MouseUp(Object sender, EventArgs e);

    /// <summary>
    /// ����ƶ�
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    public abstract void MouseMove(Object sender, EventArgs e);

    /// <summary>
    /// ���ת��ת��
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    public virtual void MouseWheel(Object sender, EventArgs e)
    {
        this.OnMouseWheel(e);
    }

    /// <summary>
    /// ������
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    public virtual void MouseEnter(Object sender, EventArgs e)
    {
        this.OnMouseEnter(e);
    }

    /// <summary>
    /// ����뿪
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    public virtual void MouseLeave(Object sender, EventArgs e)
    {
        this.OnMouseLeave(e);
    }

    #endregion

    #region Protected Functions

    protected virtual void OnMouseLButtonDown(EventArgs e) { }
    protected virtual void OnMouseLButtonUp(EventArgs e) { }
    protected virtual void OnMouseLDoubleClick(EventArgs e) { }

    protected virtual void OnMouseRButtonDown(EventArgs e) { }
    protected virtual void OnMouseRButtonUp(EventArgs e) { }
    protected virtual void OnMouseRDoubleClick(EventArgs e) { }

    protected virtual void OnMouseMove(EventArgs e) { }
    protected virtual void OnMouseHover(EventArgs e) { }
    protected virtual void OnMouseDrag(EventArgs e) { }

    protected abstract void OnMouseWheel(EventArgs e);
    protected abstract void OnMouseEnter(EventArgs e);
    protected abstract void OnMouseLeave(EventArgs e);


    #endregion

    #region ICloneable ��Ա

    public new object Clone()
    {
        return null;
    }

    #endregion

    #region IDisposable ��Ա

    public override void Dispose()
    {
    }

    #endregion
}


*/