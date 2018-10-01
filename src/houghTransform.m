%% Hough Transform - Matlab Code

%% Clear Workspace and Command Window | Close All Windows
clear all
clc
close all

%% Read the Image
I = imread('../images/png/building.png');
I  = rgb2gray(I);

%% Apply Canny
BW = edge(I,'canny');

%% Calculate the Hough Transform
[H,Theta,Rho] = hough(BW);

%% Plot the Image
figure
imshow(rescale(H));
%imshow(rescale(H),'XData',Theta,'YData',Rho,'InitialMagnification','fit');
%imshow(H);
title('Hough Transform Matrix');
xlabel('\theta')
ylabel('\rho');
axis on, axis normal;
%colormap(gca,hot)