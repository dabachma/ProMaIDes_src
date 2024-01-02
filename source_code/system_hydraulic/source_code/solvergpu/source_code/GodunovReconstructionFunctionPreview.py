import matplotlib.pyplot as plt
from matplotlib.widgets import Slider

VERY_SMALL = 1e-4
DOMAIN_DIR_N = 0
DOMAIN_DIR_E = 1
DOMAIN_DIR_S = 2
DOMAIN_DIR_W = 3

def update(val):
    # Update the heights of the rectangles based on the slider values
    rect1.set_height(slider1.val)
    rect2.set_height(slider2.val)

    rect3.set_y(slider1.val)
    rect4.set_y(slider2.val)

    rect3.set_height(slider3.val)
    rect4.set_height(slider4.val)

    # Redraw the figure
    fig.canvas.draw_idle()


    left_opt_z = slider1.val
    right_opt_z = slider2.val
    left_opt_h = slider3.val
    right_opt_h = slider4.val
    dike_z      = slider6.val


    ## Poleni Adjustments

    flowDepth = left_opt_h       + left_opt_z
    flowDepthNeigh = right_opt_h + right_opt_z
 
    if (flowDepth <= dike_z and flowDepthNeigh <= dike_z):
        print("case 1")
        right_opt_z = dike_z
        right_opt_h = 0.0

    if (flowDepth > dike_z and flowDepthNeigh <= dike_z):
        print("case 2")
        left_opt_z = dike_z
        left_opt_h = flowDepth - dike_z
        
    if (flowDepth <= dike_z and flowDepthNeigh > dike_z):
        print("case 3")
        right_opt_z = dike_z
        right_opt_h = flowDepthNeigh - dike_z
        
    if (flowDepth > dike_z and flowDepthNeigh > dike_z):
        print("case 4")
        if dike_z > left_opt_z:
            left_opt_z = dike_z
            left_opt_h = flowDepth - dike_z
        if dike_z > right_opt_z:
            right_opt_z = dike_z
            right_opt_h = flowDepthNeigh - dike_z

    ######################################



    """
    pStateLeft  = [left_opt_z  + left_opt_h  , 0 , 0 , 0]
    pStateRight = [right_opt_z + right_opt_h , 0 , 0 , 0]
    dBedLeft = left_opt_z
    dBedRight = right_opt_z
    ucDirection = slider5.val

    ucStop = 0;
    dDepthL = pStateLeft[0] - dBedLeft;
    dDepthR = pStateRight[0] - dBedRight;

    pReconstructionLeft =[pStateLeft[0],dDepthL, pStateLeft[2], pStateLeft[3], 0.0 if dDepthL < VERY_SMALL else pStateLeft[2] / dDepthL, 0.0 if dDepthL < VERY_SMALL else pStateLeft[3] / dDepthL, dBedLeft, 0.0]
    
    pReconstructionRight =[pStateRight[0], dDepthR,pStateRight[2],pStateRight[3], 0.0 if dDepthR < VERY_SMALL else pStateRight[2] / dDepthR, 0.0 if dDepthR < VERY_SMALL else pStateRight[3] / dDepthR ,dBedRight, 0.0]

    dBedMaximum = pReconstructionLeft[6] if pReconstructionLeft[6] > pReconstructionRight[6] else pReconstructionRight[6]
    dShiftV = dBedMaximum - (pStateLeft[0] if ucDirection < DOMAIN_DIR_S else pStateRight[0])
    if (dShiftV < 0.0):
        dShiftV = 0.0;

    pReconstructionLeft[1] = (pStateLeft[0] - dBedMaximum) if pStateLeft[0] - dBedMaximum > 0.0 else 0.0
    pReconstructionLeft[0] = pReconstructionLeft[1] + dBedMaximum;
    pReconstructionLeft[2] = pReconstructionLeft[1] * pReconstructionLeft[4];
    pReconstructionLeft[3] = pReconstructionLeft[1] * pReconstructionLeft[5];

    pReconstructionRight[1] = (pStateRight[0] - dBedMaximum) if pStateRight[0] - dBedMaximum > 0.0 else 0.0
    pReconstructionRight[0] = pReconstructionRight[1] + dBedMaximum;
    pReconstructionRight[2] = pReconstructionRight[1] * pReconstructionRight[4];
    pReconstructionRight[3] = pReconstructionRight[1] * pReconstructionRight[5];

    print("ucDirection: " + str(ucDirection))

    if ucDirection == DOMAIN_DIR_N:
        if (pReconstructionLeft[1] <= VERY_SMALL  and  pStateLeft[3] > 0.0):
            ucStop=ucStop+1
        if (pReconstructionRight[1] <= VERY_SMALL  and  pReconstructionLeft[5] < 0.0):
            ucStop=ucStop+1
            pReconstructionLeft[5] = 0.0
        if (pReconstructionLeft[1] <= VERY_SMALL  and  pReconstructionRight[5] > 0.0):
            ucStop=ucStop+1
            pReconstructionRight[5] = 0.0

    if ucDirection == DOMAIN_DIR_S:
        if (pReconstructionRight[1] <= VERY_SMALL  and  pStateRight[3] < 0.0):
            ucStop=ucStop+1
        if (pReconstructionRight[1] <= VERY_SMALL  and  pReconstructionLeft[5] < 0.0):
            ucStop=ucStop+1
            pReconstructionLeft[5] = 0.0
        if (pReconstructionLeft[1] <= VERY_SMALL   and  pReconstructionRight[5] > 0.0):
            ucStop=ucStop+1
            pReconstructionRight[5] = 0.0

    if ucDirection == DOMAIN_DIR_E:
        if pReconstructionLeft[1] <= VERY_SMALL   and  pStateLeft[2] > 0.0:
            ucStop=ucStop+1
        if pReconstructionRight[1] <= VERY_SMALL  and  pReconstructionLeft[4] < 0.0:
            ucStop=ucStop+1
            pReconstructionLeft[4] = 0.0
        if pReconstructionLeft[1] <= VERY_SMALL   and  pReconstructionRight[4] > 0.0:
            ucStop=ucStop+1
            pReconstructionRight[4] = 0.0

    if ucDirection == DOMAIN_DIR_W:
        if (pReconstructionRight[1] <= VERY_SMALL  and  pStateRight[2] < 0.0):
            ucStop=ucStop+1
        if (pReconstructionRight[1] <= VERY_SMALL  and  pReconstructionLeft[4] < 0.0):
            ucStop=ucStop+1
            pReconstructionLeft[4] = 0.0
        if (pReconstructionLeft[1] <= VERY_SMALL   and  pReconstructionRight[4] > 0.0):
            ucStop=ucStop+1
            pReconstructionRight[4] = 0.0

    pReconstructionLeft[6] = dBedMaximum - dShiftV
    pReconstructionRight[6] = dBedMaximum - dShiftV
    pReconstructionLeft[0] -= dShiftV
    pReconstructionRight[0] -= dShiftV

    left_opt_z = pReconstructionLeft[6]
    left_opt_h = pReconstructionLeft[1]
    right_opt_z = pReconstructionRight[6]
    right_opt_h = pReconstructionRight[1]

    print("ucStop:" + str(ucStop))

    """
    r_rect1.set_height(left_opt_z)
    r_rect2.set_height(right_opt_z)

    r_rect3.set_y(left_opt_z)
    r_rect4.set_y(right_opt_z)

    r_rect3.set_height(left_opt_h)
    r_rect4.set_height(right_opt_h)

    dike.set_height(dike_z)

    # Redraw the figure
    fig.canvas.draw_idle()



# Create the main figure
fig, ax = plt.subplots()
plt.subplots_adjust(bottom=0.3)

# Create rectangles
rect1 = plt.Rectangle((0, 0), 0.5, 1.0, color='brown')
rect2 = plt.Rectangle((0.5, 0), 0.5, 1.0, color='brown')

rect3 = plt.Rectangle((0, 1.0), 0.5, 1.0, color='blue')
rect4 = plt.Rectangle((0.5, 1.0), 0.5, 1.0, color='blue')

dike = plt.Rectangle((0.48, 0), 0.04, 1.0, color='black')

# Create rectangles
r_rect1 = plt.Rectangle((2.0, 0), 0.5, 1.0, color='brown')
r_rect2 = plt.Rectangle((2.5, 0), 0.5, 1.0, color='brown')

r_rect3 = plt.Rectangle((2.0, 1.0), 0.5, 1.0, color='blue')
r_rect4 = plt.Rectangle((2.5, 1.0), 0.5, 1.0, color='blue')

# Add rectangles to the plot
ax.add_patch(rect1)
ax.add_patch(rect2)
ax.add_patch(rect3)
ax.add_patch(rect4)
ax.add_patch(dike)
ax.add_patch(r_rect1)
ax.add_patch(r_rect2)
ax.add_patch(r_rect3)
ax.add_patch(r_rect4)

# Set plot limits
ax.set_xlim(0, 3.0)
ax.set_ylim(0, 10.0)

# Add sliders
l_z = plt.axes([0.15, 0.0, 0.2, 0.03], facecolor='lightgoldenrodyellow')
r_z = plt.axes([0.6, 0.0, 0.2, 0.03], facecolor='lightgoldenrodyellow')
l_h = plt.axes([0.15, 0.05, 0.2, 0.03], facecolor='lightgoldenrodyellow')
r_h = plt.axes([0.6, 0.05, 0.2, 0.03], facecolor='lightgoldenrodyellow')
d_d = plt.axes([0.25, 0.2, 0.5, 0.03], facecolor='lightgoldenrodyellow')
m_d = plt.axes([0.25, 0.15, 0.5, 0.03], facecolor='lightgoldenrodyellow')

slider1 = Slider(l_z, 'Left opt_z', 0.0, 5.0, valinit=1.0)
slider2 = Slider(r_z, 'Right opt_z', 0.0, 5.0, valinit=1.0)
slider3 = Slider(l_h, 'Left opt_h', 0.0, 5.0, valinit=1.0)
slider4 = Slider(r_h, 'Right opt_h', 0.0, 5.0, valinit=1.0)
slider5 = Slider(d_d, 'Direction', 0, 3,valstep=1, valinit=0)
slider6 = Slider(m_d, 'Dike Height', 0, 5.0, valinit=0)

# Attach the update function to the sliders
slider1.on_changed(update)
slider2.on_changed(update)
slider3.on_changed(update)
slider4.on_changed(update)
slider5.on_changed(update)
slider6.on_changed(update)

# Display the plot
plt.show()
