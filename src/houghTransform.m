clear all
close all
clc

I = imread('../images/lines.png');
%I  = rgb2gray(I);

BW = edge(I,'canny');

[H,Theta,Rho] = hough(BW);

 figure
 imshow(rescale(H),'XData',Theta,'YData',Rho,...
    'InitialMagnification','fit');
 title('Limited Theta Range Hough Transform of Gantrycrane Image');
 xlabel('\theta')
 ylabel('\rho');
 axis on, axis normal;
 colormap(gca,hot)