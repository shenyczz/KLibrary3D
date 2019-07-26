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
        m_pointSequence = new List<POINT>();   // 点序列(绘制图形使用)
        IsFinishedGeometry = true;              // 是否完成图形(初始状态为完成图形)
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
    /// 当前视觉
    /// </summary>
    public IVision CurrentVision { get; set; }

    /// <summary>
    /// 鼠标最后位置 LastMousePosition
    /// </summary>
    public POINT LastPoint { get; set; }

    public POINT RLastPoint { get; set; }

    /// <summary>
    /// 鼠标左键按下时位置(用于拖动)
    /// </summary>
    public POINT LButtonPoint { get; set; }

    /// <summary>
    /// 鼠标右键按下时位置
    /// </summary>
    public POINT RButtonPoint { get; set; }

    /// <summary>
    /// 鼠标拖动
    /// </summary>
    public Boolean IsDragging { get; set; }

    /// <summary>
    /// 滚轮
    /// </summary>
    public Boolean IsWheeling { get; set; }

    /// <summary>
    /// 是否双击
    /// </summary>
    public Boolean IsDoubleClick { get; set; }

    /// <summary>
    /// 左键按下
    /// </summary>
    public Boolean IsLeftButtonDown { get; set; }

    /// <summary>
    /// 右键按下
    /// </summary>
    public Boolean IsRightButtonDown { get; set; }

    /// <summary>
    /// 完成图形（绘制折线等多点图形时用到）
    /// </summary>
    public Boolean IsFinishedGeometry { get; set; }

    #endregion


    #region Public Functions - Mouse Events handdler

    /// <summary>
    /// 鼠标按下
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    public abstract void MouseDown(Object sender, EventArgs e);

    /// <summary>
    /// 鼠标释放
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    public abstract void MouseUp(Object sender, EventArgs e);

    /// <summary>
    /// 鼠标移动
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    public abstract void MouseMove(Object sender, EventArgs e);

    /// <summary>
    /// 鼠标转轮转动
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    public virtual void MouseWheel(Object sender, EventArgs e)
    {
        this.OnMouseWheel(e);
    }

    /// <summary>
    /// 鼠标进入
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    public virtual void MouseEnter(Object sender, EventArgs e)
    {
        this.OnMouseEnter(e);
    }

    /// <summary>
    /// 鼠标离开
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

    #region ICloneable 成员

    public new object Clone()
    {
        return null;
    }

    #endregion

    #region IDisposable 成员

    public override void Dispose()
    {
    }

    #endregion
}


*/